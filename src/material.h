//
// Created by Axel Sanner on 08/07/2025.
//

#include "glm/glm.hpp"

#ifndef MATERIAL_H
#define MATERIAL_H
#define MAX_MATERIALS 512
#define PN_POINT_COUNT 256


enum MaterialType {
    LAMBERTIAN,
    METAL,
    DIELECTRIC,
    LAMB_METAL,
    SOLID_TEXTURE,
    CHECKER_TEXTURE,
    NOISE_TEXTURE
};

std::vector<int> matType;

struct pak_mat {
    glm::vec4 albedo[MAX_MATERIALS];
    glm::vec4 roughness[MAX_MATERIALS];
    glm::vec4 fuzz[MAX_MATERIALS];
    glm::vec4 ior[MAX_MATERIALS];
};

struct ConstantTexture {
    glm::vec4 color;
};

struct CheckTexture {
    glm::vec4 odd;
    glm::vec4 even;
};

struct Material {
    glm::vec3 albedo;
    float roughness;
    float fuzz;
    float ior;
};

struct CreateMaterial{
    static Material lambertian(const glm::vec3& albedo) {
        matType.push_back(LAMBERTIAN);
        return {albedo, 0.0f, 0.0f, 0.0f};
    }

    static Material metal(const glm::vec3& albedo, const float fuzz) {
        matType.push_back(METAL);
        return {albedo,0.0f, fuzz, 0.0f};
    }

    static Material dielectric(const float ior) {
        matType.push_back(DIELECTRIC);
        return {glm::vec3(1.0f),0.0f, 0.0f, ior};
    }

    static ConstantTexture ctex (const glm::vec4& albedo)
    {
        matType.push_back(SOLID_TEXTURE);
        return {albedo};
    }

    static CheckTexture checktex(const glm::vec4& odd, const glm::vec4 even)
    {
        //matType.push_back(CHECKER_TEXTURE);
        return CheckTexture{.odd = odd, .even = even};
    }

    static double random_double() {
        // Returns a random real in [0,1).
        return std::rand() / (RAND_MAX + 1.0);
    }
};

#endif //MATERIAL_H
