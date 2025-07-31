//
// Created by Axel Sanner on 21/11/2023.
//

#ifndef MAIN_SPHERE_H
#define MAIN_SPHERE_H


#include "glm/glm.hpp"

class Sphere  {
public:
    glm::vec3 center;
    float radius;
    int material_index;
    int texture_index;
    int material_type;
} __attribute__((aligned (16)));

#endif //MAIN_SPHERE_H
