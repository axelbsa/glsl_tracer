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

void createRandomWorld(std::vector<Sphere> &world, std::vector<Material> &materials)
{
    int material_index = 0;
    // Ground sphere
    Material lambertian_0 = Material::lambertian(glm::vec3(0.5f, 0.5f, 0.5f));
    Sphere ground{glm::vec3(0,-1000,-1), 1000, material_index++};
    materials.push_back(lambertian_0);
    world.push_back(ground);

    int i = 5;
    for (int a = -i; a < i; a++ ) {
        for (int b = -i; b < i; b++ ) {
            float choose_mat = random_double();
            glm::vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ( (glm::length(center - glm::vec3(4.0f, 0.2f, 0))) > 0.9) {
                if (choose_mat < 0.8) {  // Lamertian, aka diffuse
                    Material mat = Material::lambertian(
                            glm::vec3(
                                    random_double() * random_double(),
                                    random_double() * random_double(),
                                    random_double() * random_double()));
                    Sphere sphere{center, 0.2f, material_index++};
                    materials.push_back(mat);
                    world.push_back(sphere);
                } else if (choose_mat < 0.95) { // metal
                    Material mat = Material::metal(
                            glm::vec3(
                                    0.5f * (1 + random_double()),
                                    0.5f * (1 + random_double()),
                                    0.5f * (1 + random_double())
                                    ),
                                    0.5 * random_double());
                    Sphere sphere{center, 0.2f, material_index++};
                    materials.push_back(mat);
                    world.push_back(sphere);
                } else { // Else == glass
                    Material mat = Material::dielectric(1.5f);
                    Sphere sphere{center, 0.2f, material_index++};
                    materials.push_back(mat);
                    world.push_back(sphere);
                }
            }
        }
    }
    Material m_big_glass = Material::dielectric(1.5f);
    Sphere s_big_glass{glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, material_index++};
    materials.push_back(m_big_glass);
    world.push_back(s_big_glass);

    Material m_big_diffuse = Material::lambertian(glm::vec3(0.4f, 0.2f, 0.1f));
    Sphere s_big_diffuse{glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, material_index++};
    materials.push_back(m_big_diffuse);
    world.push_back(s_big_diffuse);

    Material m_big_metal = Material::metal(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f);
    Sphere s_big_metal{glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, material_index++};
    materials.push_back(m_big_metal);
    world.push_back(s_big_metal);
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
    window w(1280, 720);
   // window w(1920, 1080);
    //window w(4000, 4000);
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

    glm::vec3 lookfrom = glm::vec3(13.0f, 3.0f, 3.14f);
    glm::vec3 lookat = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 vup = glm::vec3(0.0f, 1.0f, 0.0f);
    float dist_to_focus = glm::length(lookfrom - lookat);
    Camera cam(lookfrom, lookat, vup, 20, (float)w.framebuffer_width / (float)w.framebuffer_height, 0.0f, dist_to_focus, &w);

    std::vector<Sphere> world;
    std::vector<Material> materials;
    createRandomWorld(world, materials);

    // Material setup
/*
    Material lambertian_0 = Material::lambertian(glm::vec3(0.1f, 0.2f, 0.5f));
    Material lambertian_1 = Material::lambertian(glm::vec3(0.8f, 0.8f, 0.0f));

    Material metal_2 = Material::metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    Material metal_3 = Material::metal(glm::vec3(0.8f, 0.8f, 0.8f), 1.0f);
    Material dielectric_4 = Material::dielectric(1.5);
    Material dielectric_5 = Material::dielectric(1.5);

    Material lambertian_6 = Material::lambertian(glm::vec3(0.0f, 0.0f, 1.0f));
    Material lambertian_7 = Material::lambertian(glm::vec3(1.0f, 0.0f, 0.0f));

    // Geometry setup
    Sphere sphere0{glm::vec3(0,-0.0f,-1.2), 0.5, 0};
    Sphere sphere1{glm::vec3(0,-100.5,-1), 100, 1};

    Sphere sphere2{glm::vec3(1,-0.0f,-1), 0.5, 2};

    Sphere sphere3{glm::vec3(-1,-0.0f,-1), 0.5, 4};
    Sphere sphere4{glm::vec3(-1,-0.0f,-1), -0.4, 5};

    float R = cos(M_PI/4);
    Sphere sphere5{glm::vec3(-R,0.0f,-1), R, 6};
    Sphere sphere6{glm::vec3( R,0.0f,-1), R, 7};
*/

    // Add spheres to list
/*
    std::vector<Sphere> world = {
        sphere0,
        sphere1,
        sphere2,
        sphere3,
        sphere4,
    };

    // Add materials to list
    std::vector<Material> materials = {
        lambertian_0,
        lambertian_1,
        metal_2,
        metal_3,
        dielectric_4,
        dielectric_5,
        lambertian_6,
        lambertian_7,
    };
*/

//    fprintf(stderr, "Sphere0: radius %f\n", sphere0.radius);
//    fprintf(stderr, "Sphere1: radius %f\n", sphere1.radius);
//
//    fprintf(stderr, "Sphere0: position %f\n", sphere0.center.y);
//    fprintf(stderr, "Sphere1: position %f\n", sphere1.center.y);
//
//    fprintf(stderr, "Sphere0: material %d\n", sphere0.material_index);
//    fprintf(stderr, "Sphere1: material %d\n", sphere1.material_index);
    // get texture uniform location
//    GLint texture_location = glGetUniformLocation(shader_program, "tex");
//    GLint props_location = glGetUniformLocation(shader_program, "props");

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

/*
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
*/

    int frameCount = 1;
    int previousFrameCount = 0;
    int currentAccumIndex = 0;

    double currentTime = 0.0f;
    double delta = 0.0f;
    double previousTime = glfwGetTime();

    while(!glfwWindowShouldClose(w.getGLFWWindow())) {
        glfwPollEvents();

        currentTime = glfwGetTime();
        delta = currentTime - previousTime;

        lookfrom.x -= 0.01f;
        //lookfrom.y += 0.005f;
        //lookfrom.z -= 0.05f;
        Camera cam(lookfrom, lookat, vup, 20, (float)w.framebuffer_width / (float)w.framebuffer_height, 1/frameCount, dist_to_focus, &w);
        frameCount = 1;
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
        s.setVec3("cam.lower_left_corner", cam.lower_left_corner);
        s.setVec3("cam.horizontal", cam.horizontal);
        s.setVec3("cam.vertical", cam.vertical);
        s.setVec3("cam.origin", cam.origin);
        s.setVec3("cam.u", cam.u);
        s.setVec3("cam.v", cam.v);
        s.setFloat("cam.lens_radius", cam.lens_radius);

        s.setMaterials(materials);
        s.setSpheres(world);
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
        if ( delta >= 1.0 )
        {
            previousFrameCount = frameCount - previousFrameCount;
            char title [256] = {"\0"};
            snprintf ( title, 255,"%s - [FPS: %3.4f]", "GLSL RTIW", (float)previousFrameCount / delta );
            glfwSetWindowTitle (w.getGLFWWindow(), title);

            previousTime = currentTime;
        }

        // finally swap buffers
        glfwSwapBuffers(w.getGLFWWindow());

        // Ping-pong for next frame
        currentAccumIndex = 1 - currentAccumIndex;
        frameCount++;
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