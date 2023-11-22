/* OpenGL example code - Texture
 *
 * apply a texture to the fullscreen quad of "Indexed VBO"
 *
 * Autor: Jakob Progsch
 */
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#elif __linux__
#include <GL/glew.h>
#endif

#include <ostream>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include "window.h"
#include "shader.h"
#include "Sphere.h"


//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//}

struct ray_cam {
    glm::vec3 lower_left_corner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 origin;
};

int main() {
    window w(1200, 600);
    w.init();
    w.setWindowHints(GLFW_CONTEXT_VERSION_MAJOR, 4);
    w.setWindowHints(GLFW_CONTEXT_VERSION_MINOR, 1);
    w.setWindowHints(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    w.setWindowHints(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //glfwSetErrorCallback(error_callback);
    //glfwSetKeyCallback(window, key_callback);

    w.create();

    Shader s("glsl/vertex.vert", "glsl/fragment.frag");

    ray_cam r{
            glm::vec3(-2.0f, -1.0f, -1.0f),
            glm::vec3(4.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 2.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
    };

    Sphere sphere0{glm::vec3(0,0,-1), 0.5};
    Sphere sphere1{glm::vec3(0,-100.5,-1), 100};

    fprintf(stderr, "Sphere0: radius %f\n", sphere0.radius);
    fprintf(stderr, "Sphere1: radius %f\n", sphere1.radius);

    // get texture uniform location
//    GLint texture_location = glGetUniformLocation(shader_program, "tex");
//    GLint props_location = glGetUniformLocation(shader_program, "props");

    // vao and vbo handle
    GLuint vao, vbo, ibo;

    // generate and bind the vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // generate and bind the vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // data for a fullscreen quad (this time with texture coords)
    GLfloat vertexData[] = {
        //  X     Y     Z           U     V
        1.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
        -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
        1.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
        -1.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
    }; // 4 vertices with 5 components (floats) each

    // fill with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, vertexData, GL_STATIC_DRAW);


    // set up generic attrib pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));


    // generate and bind the index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    GLuint indexData[] = {
        0,1,2, // first triangle
        2,1,3, // second triangle
    };

    // fill with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);

    // "unbind" vao
    glBindVertexArray(0);

    // texture handle
    GLuint texture;

    // generate texture
    glGenTextures(1, &texture);

    // bind the texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // create some image data
    std::vector<GLubyte> image(4*w.width*w.height);
    for(uint32_t j = 0;j<w.height;++j) {
        for(uint32_t i = 0;i<w.width;++i) {
            size_t index = j*w.width + i;
            image[4*index + 0] = 0xFF*(j/10%2)*(i/10%2); // R
            image[4*index + 1] = 0xFF*(j/13%2)*(i/13%2); // G
            image[4*index + 2] = 0xFF*(j/17%2)*(i/17%2); // B
            image[4*index + 3] = 0xFF;                   // A
        }
    }

    //float myFloats[2] = {static_cast<float>(w.width), static_cast<float>(w.height)};
    glm::vec2 myFloats(w.width, w.height);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // set texture content
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w.width, w.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    double previousTime = glfwGetTime();
    int frameCount = 0;

    while(!glfwWindowShouldClose(w.getGLFWWindow())) {
        glfwPollEvents();

        // clear first
        glClear(GL_COLOR_BUFFER_BIT);

        // use the shader program
        s.use();

        s.setInt("NUM_SPHERES", 2);

        // Camera Values
        s.setVec3("cam.lower_left_corner", r.lower_left_corner);
        s.setVec3("cam.horizontal", r.horizontal);
        s.setVec3("cam.vertical", r.vertical);
        s.setVec3("cam.origin", r.origin);

        // Sphere values
        s.setVec3("sphere[0].center", sphere0.center);
        s.setFloat("sphere[0].radius", sphere0.radius);
        s.setVec3("sphere[1].center", sphere1.center);
        s.setFloat("sphere[1].radius", sphere1.radius);

        // bind texture to texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        s.setVec2("props", myFloats);

        // bind the vao
        glBindVertexArray(vao);

        // draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // check for errors
        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {
            std::cerr << error << std::endl;
            break;
        }

        double currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        double delta = currentTime - previousTime;
        if ( delta >= 1.0 )
        {
            char title [256] = {"\0"};
            snprintf ( title, 255,"%s - [FPS: %3.4f]", "texture", (float)frameCount / delta );
            glfwSetWindowTitle (w.getGLFWWindow(), title);

            frameCount = 0;
            previousTime = currentTime;
        }

        // finally swap buffers
        glfwSwapBuffers(w.getGLFWWindow());
    }

    // delete the created objects

    glDeleteTextures(1, &texture);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    glfwDestroyWindow(w.getGLFWWindow());
    glfwTerminate();
    return 0;
}
