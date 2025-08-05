//
// Created by virt01 on 05/08/2025.
//

#ifndef PERLIN_H
#define PERLIN_H
#include <vector>
#include "utils.h"

static constexpr int point_count = 256;

struct PerlinBlock
{
    //float randfloat[point_count];
    glm::vec4 randfloat[point_count];
    glm::ivec4 perm_x[point_count];
    glm::ivec4 perm_y[point_count];
    glm::ivec4 perm_z[point_count];
};

class Perlin {
public:
    PerlinBlock pb;
    Perlin(): pb() {
        for (int i = 0; i < point_count; i++) {
            pb.randfloat[i] = glm::vec4(random_double(), random_double(), random_double(), 0);
        }

        perlin_generate_perm(pb.perm_x);
        perlin_generate_perm(pb.perm_y);
        perlin_generate_perm(pb.perm_z);
    }

    // double noise(const point3& p) const {
    //     auto i = int(4*p.x()) & 255;
    //     auto j = int(4*p.y()) & 255;
    //     auto k = int(4*p.z()) & 255;
    //
    //     return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
    // }

private:
    static void perlin_generate_perm(glm::ivec4* p) {
        for (int i = 0; i < point_count; i++)
            p[i] = glm::ivec4(i, i, i, i);

        permute(p, point_count);
    }

    static void permute(glm::ivec4* p, int n) {
        for (int i = n-1; i > 0; i--) {
            int target = random_int(0, i);
            glm::ivec4 tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }
};

#endif //PERLIN_H

