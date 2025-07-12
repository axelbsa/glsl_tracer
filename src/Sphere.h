//
// Created by Axel Sanner on 21/11/2023.
//

#ifndef MAIN_SPHERE_H
#define MAIN_SPHERE_H


#include "glm/glm.hpp"

class sphere {
public:
    glm::vec3 center;
    float radius;
    int material_index;
    int material_type;
};

#endif //MAIN_SPHERE_H
