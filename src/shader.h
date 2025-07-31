//
// Created by Axel Sanner on 13/11/2023.
//

#ifndef MAIN_SHADER_H
#define MAIN_SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#elif __linux__
#include <GL/gl.h>
#else
#include "glad/glad.h"
#endif

#include <ostream>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "material.h"
#include "glm/glm.hpp"
#include "sphere.h"
#include "camera2.h"


class Shader {
public:
    unsigned int ID;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader();

    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr) {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr) {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr) {
            const char *gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);

    }

    // Upload all data to GPU
    // TODO Remove this function, we use UBO now
    void setMaterials(const std::vector<Material> &materials) const {
        // Upload material data
        if (!materials.empty()) {
            std::vector<glm::vec3> albedos;
            std::vector<float> roughness_values;
            std::vector<int> types;
            std::vector<float> fuzz_values;
            std::vector<float> ior_values;

            for (const auto &mat: materials) {
                albedos.push_back(mat.albedo);
                roughness_values.push_back(mat.roughness);
                //types.push_back(static_cast<int>(mat.type));
                fuzz_values.push_back(mat.fuzz);
                ior_values.push_back(mat.ior);
            }

            setVec3("material_albedo", albedos[0], albedos.size());
            setFloatArray("material_roughness", roughness_values.data(), roughness_values.size());
            setIntArray("material_type", types.data(), types.size());
            setFloatArray("material_fuzz", fuzz_values.data(), fuzz_values.size());
            setFloatArray("material_ior", ior_values.data(), ior_values.size());
        }
    }

    void setSpheres(const std::vector<Sphere> &spheres) const {
        for (size_t i = 0; i < spheres.size(); ++i) {
            std::string base = "sphere[" + std::to_string(i) + "]";

            setVec3(base + ".center", spheres[i].center);
            setFloat(base + ".radius", spheres[i].radius);
            setInt(base + ".material_index", spheres[i].material_index);
            //setInt(base + ".material_type", spheres[i].material_type);
        }
        setInt("NUM_SPHERES", static_cast<int>(spheres.size()));


    }


    // activate the shader
    // ------------------------------------------------------------------------
    void use() const {
        glUseProgram(ID);
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, const bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setUInt(const std::string &name, unsigned int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setIntArray(const std::string &name, const int *values, const int count) const {
        glUniform1iv(glGetUniformLocation(ID, name.c_str()), count, values);
    }

    void setFloat(const std::string &name, const float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloatArray(const std::string &name, const float *values, const int count) const {
        glUniform1fv(glGetUniformLocation(ID, name.c_str()), count, values);
    }

    void setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void setVec3(const std::string &name, const glm::vec3 &value, const int count) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
    }

    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setCamUbo(const std::string &name, CameraBlock &camblock, unsigned int uboExampleBlock) const {
        unsigned int CameraBlockIndex = glGetUniformBlockIndex(ID, name.c_str());
        glUniformBlockBinding(ID, CameraBlockIndex, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboExampleBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(camblock), &camblock);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void setSphereUbo(const std::string &name, std::vector<Sphere> &sph, unsigned int sphereBlock) const {
        unsigned int SphereBlockIndex = glGetUniformBlockIndex(ID, name.c_str());
        glUniformBlockBinding(ID, SphereBlockIndex, 1);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphereBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Sphere) * sph.size(), sph.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        setInt("NUM_SPHERES", static_cast<int>(sph.size()));
    }

    void setMaterialUbo(
            const std::string &name, std::vector<Material> &mat,
            unsigned int MaterialBlock, std::vector<MaterialType> &matType) const {
        std::vector<int> matertial_type;

        int index = 0;
        struct pak_mat p;
        for (const auto &material: mat) {
            p.albedo[index] = glm::vec4(material.albedo, 0);
            p.roughness[index] = glm::vec4(material.roughness, 0, 0, 0);
            p.fuzz[index] = glm::vec4(material.fuzz, 0, 0, 0);
            p.ior[index] = glm::vec4(material.ior, 0, 0, 0);
            matertial_type.push_back(matType[index]);
            index += 1;
        }

        unsigned int MaterialBlockIndex = glGetUniformBlockIndex(ID, name.c_str());
        glUniformBlockBinding(ID, MaterialBlockIndex, 2);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, MaterialBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(pak_mat), &p);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        setIntArray("material_type", matertial_type.data(), matertial_type.size());
    }


//    void setConstantTextureUbo(const std::string &name, std::vector<Material> &mat, unsigned int MaterialBlock) const {
////        unsigned int MaterialBlockIndex = glGetUniformBlockIndex(ID, name.c_str());
////        glUniformBlockBinding(ID, MaterialBlockIndex, 2);
////        glBindBufferBase(GL_UNIFORM_BUFFER, 2, MaterialBlock);
////        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(pak_mat), &p);
////        glBindBuffer(GL_UNIFORM_BUFFER, 0);
//    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------


    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif //MAIN_SHADER_H
