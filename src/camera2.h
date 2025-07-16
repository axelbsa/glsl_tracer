//
// Created by Axel Sanner on 16/07/2025.
//

#ifndef MAIN_CAMERA2_H
#define MAIN_CAMERA2_H

#ifndef M_PI
#define M_PI 3.14159265
#endif

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "window.h"

class Camera {
public:
    window *w;
    const float aspect_ratio = 16.0f / 9.0f;
    int image_width;
    int image_height;

    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    Camera (float vfov, float aspect, window *win)
    {
        w = win;

        float theta = vfov * M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;

        image_width = w->width;
        image_height = static_cast<int>(image_width / aspect_ratio);

        //lower_left_corner = origin - horizontal/2.0f - vertical/2.0f - glm::vec3(0.0f, 0.0f, focal_length);
        //horizontal = glm::vec3(viewport_width, 0.0f, 0.0f);
        //vertical = glm::vec3(0.0f, viewport_height, 0.0f);

        lower_left_corner = glm::vec3(-half_width, -half_height, -1.0f);
        horizontal = glm::vec3(2 * half_width, 0.0f, 0.0f);
        vertical = glm::vec3(0.0f, 2 * half_height, 0.0f);
        origin = glm::vec3 (0.0f, 0.0f, 0.0f);
    }

    glm::vec3 origin;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 lower_left_corner;
};

#endif //MAIN_CAMERA2_H
