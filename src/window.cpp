//
// Created by Axel Sanner on 21/11/2023.
//
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#elif __linux__
#include "glad/glad.h"
#else
#include "glad/glad.h"
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "limits.h"

#include "window.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    // Update your stored framebuffer dimensions
}

static void ErrorCallback(int, const char* err_str)
{
    std::cerr << "GLFW Error: " << err_str << std::endl;
}

uint32_t window::init() {
    glfwSetErrorCallback(ErrorCallback);
    if(glfwInit() == GL_FALSE) {
        std::cerr << "failed to init GLFW" << std::endl;
        return 1;
    }
    return 0;
}

uint32_t window::create() {
    if((w = glfwCreateWindow(width, height, "", 0, 0)) == 0) {
        std::cerr << "failed to open window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    glfwSwapInterval( 0 );

    glfwSetFramebufferSizeCallback(w, framebuffer_size_callback);

#ifndef __APPLE__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD");
        return -1;
    }
#endif

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(w, &fbWidth, &fbHeight);

    // Update your class members or store these values
    framebuffer_width = fbWidth;
    framebuffer_height = fbHeight;

    // Set viewport to framebuffer size
    glViewport(0, 0, fbWidth, fbHeight);

    return 0;
}

uint32_t window::setWindowHints(uint32_t hint, uint32_t value) {
    // Check return and act on it
    glfwWindowHint(hint, value);
    return 0;
}

GLFWwindow *window::getGLFWWindow() {
    return w;
}
