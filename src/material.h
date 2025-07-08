//
// Created by Axel Sanner on 08/07/2025.
//

#include "glm/glm.hpp"

#ifndef MATERIAL_H
#define MATERIAL_H

enum MaterialType {
    LAMBERTIAN,
    METAL,
    DIELECTRIC
};

struct Material {
    glm::vec3 albedo;
    float roughness;
    float fuzz;
    float ior;
    MaterialType type;

    static Material lambertian(const glm::vec3& albedo) {
        return {albedo, 0.0f, 0.0f, 0.0f, LAMBERTIAN};
    }

    static Material metal(const glm::vec3& albedo, float fuzz) {
        return {albedo,0.0f, fuzz, 0.0f, METAL};
    }

    static Material dielectric(float ior) {
        return {glm::vec3(1.0f),0.0f, 0.0f, ior, DIELECTRIC};
    }
};



#endif //MATERIAL_H
