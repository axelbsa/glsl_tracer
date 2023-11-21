//
// Created by Axel Sanner on 21/11/2023.
//
#include <iostream>

#include <GLFW/glfw3.h>

#include "window.h"

uint32_t window::init() {
    if(glfwInit() == GL_FALSE) {
        std::cerr << "failed to init GLFW" << std::endl;
        return 1;
    }

}

uint32_t window::create() {
    if((w = glfwCreateWindow(width, height, "texture", 0, 0)) == 0) {
        std::cerr << "failed to open window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
}

uint32_t window::setWindowHints(uint32_t hint, uint32_t value) {
    glfwWindowHint(hint, value);
}

GLFWwindow *window::getGLFWWindow() {
    return w;
}

