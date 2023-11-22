//
// Created by Axel Sanner on 21/11/2023.
//
#include <iostream>

#ifdef __linux__
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "window.h"

uint32_t window::init() {
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

#ifdef __linux__
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW not initialized properly\n");
        exit(1); // or handle the error in a nicer way
    }
    if (!GLEW_VERSION_3_3) {  // check that the machine supports the 2.1 API.
        fprintf(stderr, "We dont have minimum OpenGL version\n");
        exit(1); // or handle the error in a nicer way
    }
#endif
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

