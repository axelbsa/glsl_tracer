#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include "glad/glad.h"
#endif

#include <ostream>
#include <cstdlib>
#include <time.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
//#include <gl/gl.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include "window.h"
#include "shader.h"
#include "sphere.h"
#include "material.h"
#include "camera2.h"

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//}

inline double random_double() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

void updateSpheres(std::vector<Sphere> &world, double delta_time)
{
    int randomSphere = std::rand() % world.size();
    world[2].center.y += 0.1 * delta_time;
    world[2].center.x -= 0.01 * delta_time;

    world[3].center.y -= 0.1 * delta_time;
    world[3].center.x += 0.04 * delta_time;
    world[3].center.z += 0.05 * delta_time;

}

void createRandomWorld(
        std::vector<Sphere> &world, std::vector<Material> &materials,
        std::vector<ConstantTexture> &ctex, std::vector<CheckTexture> &checktex)
{
    int material_index = 0;
    int texture_index = 0;
    CheckTexture check_tex_0 = CreateMaterial::checktex(
            glm::vec4(1, 0.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));
    // Ground sphere
    Material lambertian_0 = CreateMaterial::lambertian(glm::vec3(0.5f, 0.5f, 0.5f));
    ConstantTexture ctex_0 = CreateMaterial::ctex(glm::vec4(0.5f, 0.5f, 0.5f, 0.0f));
    Sphere ground{glm::vec3(0,-1000,-1), 1000, material_index++, 0, CHECKER_TEXTURE};
    materials.push_back(lambertian_0);
    ctex.push_back(ctex_0);
    checktex.push_back(check_tex_0);
    world.push_back(ground);

    int i = 11;
    for (int a = -i; a < i; a++ ) {
        for (int b = -i; b < i; b++ ) {
            double choose_mat = random_double();
            glm::vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ( (glm::length(center - glm::vec3(4.0f, 0.2f, 0))) > 0.9) {
                if (choose_mat < 0.8) {  // Lambertian, aka diffuse
                    Material mat = CreateMaterial::lambertian(
                            glm::vec3(
                                    random_double() * random_double(),
                                    random_double() * random_double(),
                                    random_double() * random_double()));
                    Sphere sphere{center, 0.2f, material_index++, texture_index++, LAMBERTIAN};
                    materials.push_back(mat);
                    world.push_back(sphere);
                } else if (choose_mat < 0.95) { // metal
                    Material mat = CreateMaterial::metal(
                            glm::vec3(
                                    0.5f * (1 + random_double()),
                                    0.5f * (1 + random_double()),
                                    0.5f * (1 + random_double())
                                    ),
                                    0.5 * random_double());
                    Sphere sphere{center, 0.2f, material_index++, texture_index++, METAL};
                    materials.push_back(mat);
                    world.push_back(sphere);
                } else { // Else == glass
                    Material mat = CreateMaterial::dielectric(1.5f);
                    Sphere sphere{center, 0.2f, material_index++, texture_index++, DIELECTRIC};
                    materials.push_back(mat);
                    world.push_back(sphere);
                }
            }
        }
    }
    Material m_big_glass = CreateMaterial::dielectric(1.5f);
    Sphere s_big_glass{
        glm::vec3(0.0f, 1.0f, 0.0f),
        1.0f, material_index++, texture_index++, DIELECTRIC};
    materials.push_back(m_big_glass);
    world.push_back(s_big_glass);

    Material m_big_diffuse = CreateMaterial::lambertian(glm::vec3(0.4f, 0.2f, 0.1f));
    Sphere s_big_diffuse{
        glm::vec3(-4.0f, 1.0f, 0.0f),
        1.0f, material_index++, texture_index++, LAMBERTIAN};
    materials.push_back(m_big_diffuse);
    world.push_back(s_big_diffuse);

    Material m_big_metal = CreateMaterial::metal(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f);
    Sphere s_big_metal{
        glm::vec3(4.0f, 1.0f, 0.0f),
        1.0f, material_index++, texture_index++, METAL};
    materials.push_back(m_big_metal);
    world.push_back(s_big_metal);
}

void createSimpleTestScene(
        std::vector<Sphere> &world, std::vector<Material> &materials,
        std::vector<ConstantTexture> &ctex, std::vector<CheckTexture> &checktex)
{

    ConstantTexture ctex_0 = CreateMaterial::ctex(glm::vec4(0.1f, 0.1f, 0.1f, 0.0f));
    CheckTexture check_tex_0 = CreateMaterial::checktex(
            glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
            glm::vec4(1.0f, .67f, .0f, 1.0f));
    Material dielectric_0 = CreateMaterial::dielectric(1.5);
    Material lambertian_1 = CreateMaterial::lambertian(glm::vec3(0.1f, 0.2f, 0.5f));
    Material metal_2 = CreateMaterial::metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);

    Sphere sphere0{glm::vec3(0,-0.0f,-1.2), 0.5, 1, -1, LAMBERTIAN};
    Sphere sphere1{glm::vec3(0,-100.5,-1), 100, -1, 0, CHECKER_TEXTURE};
    Sphere sphere2{glm::vec3(1,-0.0f,-1), 0.5, 2, -1, METAL};
    Sphere sphere3{glm::vec3(-1,-0.0f,-1), 0.5, 0, -1, DIELECTRIC};
    Sphere sphere4{glm::vec3(-1,-0.0f,-1), -0.499, 0, -1, DIELECTRIC};

    ctex.push_back(ctex_0);

    checktex.push_back(check_tex_0);

    materials.push_back(dielectric_0);
    materials.push_back(lambertian_1);
    materials.push_back(metal_2);

    world.push_back(sphere0);
    world.push_back(sphere1);
    world.push_back(sphere2);
    world.push_back(sphere3);
    world.push_back(sphere4);

    for (auto m: matType) {
        fprintf(stderr, "Material type: %d\n", m);
    }

    for (int i = 0; i < world.size(); i++) {
        fprintf(stderr, "Sphere mat type: %d\n", world[i].material_type);
    }
}


void createTestScene(
        std::vector<Sphere> &world, std::vector<Material> &materials,
        std::vector<ConstantTexture> &ctex, std::vector<CheckTexture> &checktex)
{

    Material lambertian_0 = CreateMaterial::lambertian(glm::vec3(0.1f, 0.2f, 0.5f));
    Material lambertian_1 = CreateMaterial::lambertian(glm::vec3(0.8f, 0.8f, 0.0f));

    ConstantTexture ctex_0 = CreateMaterial::ctex(glm::vec4(0.8f, 0.8f, 0.0f, 0.0f));
    CheckTexture check_tex_0 = CreateMaterial::checktex(
            glm::vec4(0.2f, 0.3f, 0.1f, 1.0f),
            glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    Material metal_2 = CreateMaterial::metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    Material metal_3 = CreateMaterial::metal(glm::vec3(0.8f, 0.8f, 0.8f), 1.0f);
    Material dielectric_4 = CreateMaterial::dielectric(1.5);
    Material dielectric_5 = CreateMaterial::dielectric(1.5);

    Material lambertian_6 = CreateMaterial::lambertian(glm::vec3(0.0f, 0.0f, 1.0f));
    Material lambertian_7 = CreateMaterial::lambertian(glm::vec3(1.0f, 0.0f, 0.0f));

    // Geometry setup
    Sphere sphere0{glm::vec3(0,-0.0f,-1.2), 0.5, 0, -1, LAMBERTIAN};
    Sphere sphere1{glm::vec3(0,-100.5,-1), 100, 0, 0, SOLID_TEXTURE};

    Sphere sphere2{glm::vec3(1,-0.0f,-1), 0.5, 2, -1, METAL};

    Sphere sphere3{glm::vec3(-1,-0.0f,-1), 0.5, 4,-1, DIELECTRIC};
    Sphere sphere4{glm::vec3(-1,-0.0f,-1), -0.4, 5, -1, DIELECTRIC};

    float R = cos(M_PI/4);
    Sphere sphere5{glm::vec3(-R,0.0f,-1), R, 6};
    Sphere sphere6{glm::vec3( R,0.0f,-1), R, 7};

    // Add spheres to list
    world = {
            sphere0,
            sphere1,
            sphere2,
            sphere3,
            sphere4,
    };

    checktex.push_back(check_tex_0);

    ctex = {
        ctex_0
    };

    // Add materials to list
    materials = {
            lambertian_0,
            lambertian_1,
            metal_2,
            metal_3,
            dielectric_4,
            dielectric_5,
            lambertian_6,
            lambertian_7,
    };

}

void debugCamera(Camera cam)
{

    fprintf(
            stderr,
            "Camera has: lookFrom: [%f, %f, %f]\n",
            cam.look_from.x,
            cam.look_from.y,
            cam.look_from.z
    );
    fprintf(
            stderr,
            "Camera has: lower_left: [%f, %f, %f]\n",
            cam.lower_left_corner.x,
            cam.lower_left_corner.y,
            cam.lower_left_corner.z
    );
}

int main() {

    glm::vec3 a = glm::vec3(1.0, 1.0, 1.0);
    std::cout << sizeof(a) << std::endl;
    std::cout << sizeof(struct CameraBlock) << std::endl;

    //window w(1280, 720);
    window w(1920, 1080);
    // window w(4000, 4000);
    w.init();
    w.setWindowHints(GLFW_CONTEXT_VERSION_MAJOR, 4);
    w.setWindowHints(GLFW_CONTEXT_VERSION_MINOR, 1);
    w.setWindowHints(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    w.setWindowHints(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    srand (time(NULL));

    //glfwSetErrorCallback(error_callback);
    //glfwSetKeyCallback(window, key_callback);

    w.create();

    Shader s("glsl/vertex.vert", "glsl/fragment.frag");
    Shader t("glsl/texture.vert", "glsl/texture.frag");

    //glm::vec3 lookfrom = glm::vec3(13.0f, 3.0f, 3.14f);
    glm::vec3 lookfrom = glm::vec3(0.0f, 1.5f, 5.14f);
    glm::vec3 lookat = glm::vec3(0.0f, 0.5f, 0.0f);
    glm::vec3 vup = glm::vec3(0.0f, 1.0f, 0.0f);
    float dist_to_focus = glm::length(lookfrom - lookat);
    CameraBlock camblock;
    Camera cam(lookfrom, lookat, vup, 20, (float)w.framebuffer_width / (float)w.framebuffer_height, 0.0f, dist_to_focus, &w);
    cam.createCamUBO(camblock);

    std::vector<Sphere> world;
    std::vector<Material> materials;
    std::vector<ConstantTexture> ctex;
    std::vector<CheckTexture> checktex;

    //createRandomWorld(world, materials, ctex, checktex);
    //createTestScene(world, materials, ctex, checktex);
    createSimpleTestScene(world, materials, ctex, checktex);


    // vao and vbo handle
    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // data for a fullscreen quad (this time with texture coords yay!)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)(0*sizeof(GLfloat)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)(3*sizeof(GLfloat))); // <- Legacy: ptr value is offset

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

    // Framebuffer generation
    GLuint rayTraceFBO, accumulateFBO;

    // texture handle
    GLuint currentSampleTexture, accumulatedTexture[2];
    // Create textures
    glGenTextures(1, &currentSampleTexture);
    glGenTextures(2, accumulatedTexture);

    // Setup all textures (floating point)
    for(int i = 0; i < 3; i++) {
        GLuint tex = (i == 0) ? currentSampleTexture : accumulatedTexture[i-1];
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                     w.framebuffer_width, w.framebuffer_height,
                     0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glGenFramebuffers(1, &rayTraceFBO);
    glGenFramebuffers(1, &accumulateFBO);

    // UBO buffers
    unsigned int cameraBlock;
    glGenBuffers(1, &cameraBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct CameraBlock), nullptr, GL_STATIC_DRAW); // allocate 152 bytes of memory
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    s.setCamUbo("CameraBlock", camblock, cameraBlock);

    unsigned int sphereBlock;
    glGenBuffers(1, &sphereBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, sphereBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere) * world.size(), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    unsigned int materialBlock;
    glGenBuffers(1, &materialBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, materialBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(pak_mat), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    unsigned int constantTextureBlock;
    glGenBuffers(1, &constantTextureBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, constantTextureBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ConstantTexture) * ctex.size(), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    unsigned int checkerTextureBlock;
    glGenBuffers(1, &checkerTextureBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, checkerTextureBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CheckTexture) * checktex.size(), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


/*
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
*/

    int frameCount = 1;
    int fpsCounter = 0;
    int currentAccumIndex = 0;

    double currentTime = 0.0f;
    double delta = 0.0f;
    double previousTime = glfwGetTime();

    while(!glfwWindowShouldClose(w.getGLFWWindow())) {
        glfwPollEvents();

        currentTime = glfwGetTime();
        delta = currentTime - previousTime;

        //lookfrom.x -= 0.05f;
        //lookfrom.y += 0.005f;
        //lookfrom.z -= 0.05f;
        //Camera cam(lookfrom, lookat, vup, 20, (float)w.framebuffer_width / (float)w.framebuffer_height, 1/frameCount, dist_to_focus, &w);
        //frameCount = 1;
        //debugCamera(cam);

        // clear first
        glClear(GL_COLOR_BUFFER_BIT);

        // Step 1: render current ray traced scene
        glBindFramebuffer(GL_FRAMEBUFFER, rayTraceFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, currentSampleTexture, 0);
        glViewport(0, 0, w.framebuffer_width, w.framebuffer_height);

        // The main raytracing shader 's' such a descriptive name
        s.use();
        s.setFloat("time", currentTime);
        s.setInt("frame_number", frameCount);  // Uncomment this when ready to sanple multiple frames

        // Camera Values
        cam.createCamUBO(camblock);
        s.setCamUbo("CameraBlock", camblock, cameraBlock);

        s.setMaterialUbo("MaterialBlock", materials, materialBlock);
        s.setConstantTextureUbo("ConstantTextureBlock", ctex, constantTextureBlock);
        s.setCheckerTextureUbo("CheckerTextureBlock", checktex, checkerTextureBlock);
        s.setSphereUbo("SphereBlock", world, sphereBlock);
        s.setVec2("props", glm::vec2(w.framebuffer_width, w.framebuffer_height));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Step 2: render accumulated
        glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               accumulatedTexture[currentAccumIndex], 0);
        glViewport(0, 0, w.framebuffer_width, w.framebuffer_height);

        t.use();
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentSampleTexture);
        t.setInt("currentFrame", 0);

        // Bind previous accumulated result
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, accumulatedTexture[1 - currentAccumIndex]);
        t.setInt("previousFrame", 1);
        t.setInt("frameCount", frameCount);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Step 3: blit last framebuffer to screen
        glBindFramebuffer(GL_READ_FRAMEBUFFER, accumulateFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
        glBlitFramebuffer(
                0, 0, w.framebuffer_width, w.framebuffer_height,    // Source rectangle
                0, 0, w.framebuffer_width, w.framebuffer_height,    // Destination rectangle
                GL_COLOR_BUFFER_BIT,        // Copy color buffer
                GL_NEAREST                  // No filtering needed
        );

        // check for errors
        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {
            std::cerr << error <<  std::endl;
            break;
        }

        // If a second has passed.
        //if ( currentTime - previousTime >= 1.0 )
        //{
            char title [256] = {"\0"};
            snprintf ( title, 255,"%s - [FPS: %3.4f]", "GLSL RTIW", (float)fpsCounter / delta );
            glfwSetWindowTitle (w.getGLFWWindow(), title);

            fpsCounter = 0;
        //}
        previousTime = currentTime;

        // finally swap buffers
        glfwSwapBuffers(w.getGLFWWindow());

        //updateSpheres(world, delta);

        // Ping-pong for next frame
        currentAccumIndex = 1 - currentAccumIndex;
        frameCount++;
        fpsCounter++;
    }

    // delete the created objects
    glDeleteTextures(1, &currentSampleTexture);
    glDeleteTextures(1, &accumulatedTexture[0]);
    glDeleteTextures(1, &accumulatedTexture[1]);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    glfwDestroyWindow(w.getGLFWWindow());
    glfwTerminate();
    return 0;
}
