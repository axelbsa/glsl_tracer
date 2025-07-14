//
// Created by Axel Sanner on 21/11/2023.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <stdint.h>

#include <GLFW/glfw3.h>


class window {
public:
    uint32_t width = 640;
    uint32_t height = 480;

    window(uint32_t w, uint32_t h): width(w), height(h) {}

    uint32_t init();
    uint32_t create();
    uint32_t setWindowHints(uint32_t A, uint32_t B);

    GLFWwindow *getGLFWWindow();
    int framebuffer_width;
    int framebuffer_height;

private:
    GLFWwindow *w;

};


#endif //MAIN_WINDOW_H
