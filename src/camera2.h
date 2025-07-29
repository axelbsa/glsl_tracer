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

struct CameraBlock {
    glm::vec4 lower_left_corner;   // 12
    glm::vec4 horizontal;          // 12
    glm::vec4 vertical;            // 12
    glm::vec4 origin;              // 12
    glm::vec4 u;                   // 12
    glm::vec3 v;                   // 12
    float lens_radius;             // 4
                                   // Total 16 * 6 == 96
};

class Camera {
public:
    window *win;

    Camera (glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 vup, float vfov, float aspect, float aparture, float focus_dist, window *win)
    {
        this->win = win;
        look_at = lookat;
        look_from = lookfrom;
        lens_radius = aparture / 2;
        float theta = vfov * M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;

        origin = lookfrom;

        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(glm::cross(vup, w));
        v = glm::cross(w, u);

//        lower_left_corner = origin - horizontal/2.0f - vertical/2.0f - glm::vec3(0.0f, 0.0f, focal_length);
//        horizontal = glm::vec3(viewport_width, 0.0f, 0.0f);
//        vertical = glm::vec3(0.0f, viewport_height, 0.0f);

        // lower_left_corner = glm::vec3(-half_width, -half_height, -1.0f);
        lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
        horizontal = 2 * half_width * focus_dist * u;
        vertical = 2 * half_height * focus_dist * v;
    }

    void createCamUBO(CameraBlock &cam) {
        cam.lower_left_corner = glm::vec4 (this->lower_left_corner, 0.0f);
        cam.horizontal = glm::vec4(this->horizontal, 0.0f);
        cam.vertical = glm::vec4 (this->vertical, 0.0f);
        cam.origin = glm::vec4 (this->origin, 0.0f);
        cam.u = glm::vec4 (this->u, 0.0f);
        cam.v = glm::vec4 (this->v, 0.0f);
        cam.lens_radius = this->lens_radius;
    }

    glm::vec3 origin;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 lower_left_corner;
    glm::vec3 look_at;
    glm::vec3 look_from;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;
    float lens_radius;
};

#endif //MAIN_CAMERA2_H
