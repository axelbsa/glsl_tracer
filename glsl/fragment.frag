#version 410

#define MAX_MATERIALS 512
#define MAX_FLOAT 1e5
#define FLT_MAX 3.402823466e+38
#define PHI 1.61803398874989484820459
#define PI 3.1415926535897932385
#define TAU 2. * PI
#define MAX_SPHERES 2048

// Material types as mapped on the cpu
#define LAMBERTIAN 0
#define METAL 1
#define DIELECTRIC 2
#define LAMB_METAL 3
#define SOLID_TEXTURE 4
#define CHECKER_TEXTURE 5
#define NOISE_TEXTURE 6


struct Camera {
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    vec3 u;
    vec3 v;
    float lens_radius;
};

struct Sphere {
    vec3 center;
    float radius;
    int material_index;
    int texture_index;
    int material_type;
};

struct Ray {
    vec3 A;
    vec3 B;
};

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    int material_index;
    int texture_index;
    int material_type;
};

struct ConstantTexture {
    vec3 color;
};

struct CheckTexture {
    vec3 odd;
    vec3 even;
};

uniform float time;
uniform sampler2D tex; // texture uniform
uniform vec2 props;
uniform int NUM_SPHERES;
uniform int frame_number;

layout (std140) uniform CameraBlock
{
    Camera cam;
};

layout (std140) uniform SphereBlock
{
    Sphere sphere2[MAX_SPHERES];
};

layout (std140) uniform ConstantTextureBlock
{
    ConstantTexture ctex[MAX_MATERIALS];
};

layout (std140) uniform CheckTextureBlock
{
    CheckTexture checktex[MAX_MATERIALS];
};

// This is a little stupid, but values comes in quadruplets, albedo[0], roughness[0], fuzz[0], ior[0] defines 1 material
layout (std140) uniform MaterialBlock
{
    uniform vec3 material_albedo[MAX_MATERIALS];
    uniform float material_roughness[MAX_MATERIALS];
    uniform float material_fuzz[MAX_MATERIALS];
    uniform float material_ior[MAX_MATERIALS];
};
// The idea is to have this array of int's be a lookup table for if the material type is lambertian, metal, etc
// ex: material_type = [2,0,1,2,3,1,0, ...] that way we don't need the material_type in the sphere struct
//uniform int material_type[MAX_MATERIALS]; // 0=lambertian, 1=metal, 2=dielectric

in vec2 ftexcoord;
in vec4 gl_FragCoord;

layout(location = 0) out vec4 FragColor;

float g_seed = 0.25;

vec3 linearToSRGB(vec3 color) {
    bvec3 cutoff = lessThan(color, vec3(0.0031308));
    vec3 higher = vec3(1.055) * pow(color, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = color * vec3(12.92);
    return mix(higher, lower, cutoff);
}

vec3 reinhard(vec3 x) {
    return x / (1.0 + x);
}

vec3 aces(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 filmic(vec3 x) {
    vec3 X = max(vec3(0.0), x - 0.004);
    vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
    return pow(result, vec3(2.2));
}

vec3 lottes(vec3 x) {
    const vec3 a = vec3(1.6);
    const vec3 d = vec3(0.977);
    const vec3 hdrMax = vec3(8.0);
    const vec3 midIn = vec3(0.18);
    const vec3 midOut = vec3(0.267);

    const vec3 b =
    (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
    ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const vec3 c =
    (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
    ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

float squared_length(vec3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

float random(vec2 st)
{
    //https://stackoverflow.com/questions/53500550/generating-a-pseudo-random-number-between-0-an-1-glsl-es
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float drand48(vec2 co) {
    return 2 * fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453) - 1;
}

float random_golden (vec2 st) {
    return fract(tan(distance(st*PHI, st)*g_seed)*st.x);
}

float random_float(inout uint state) {
    state = state * 1664525u + 1013904223u;  // LCG constants
    return float(state) * (1.0 / 4294967296.0);  // Convert to [0,1)
}

float random_float2(inout uint state) {
    state ^= (state << 13u);
    state ^= (state >> 17u);
    state ^= (state << 5u);
    return float(state) * (1.0 / 4294967296.0);
}

float RandomValue(inout uint state)
{
    state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28 ) + 4)) ^ state ) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0f;
}

float schlick(float cosine, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

vec3 random_in_unit_sphere(vec3 p)
{
    int n = 0;
    do {
        //p = 2.0f * vec3(random(state2), random(state2), random(state2)) - vec3(1.0f);
        p = vec3(drand48(p.xy), drand48(p.zy), drand48(p.xz));
        n++;
    } while (squared_length(p) >= 1.0 && n < 3);
    return p;
}

vec3 random_in_unit_disk(inout uint state) {
    vec3 p;
    do {
        p = 2.0f * vec3(RandomValue(state), RandomValue(state), 0.0f) - vec3(1.0f, 1.0f, 1.0f);
    } while (squared_length(p) >= 1.0);
    return p;
}

vec3 random_in_unit_sphere2(inout uint state)
{
    vec3 p;
    do {
        p = 2.0f * vec3(RandomValue(state), RandomValue(state), RandomValue(state)) - vec3(1.0f, 1.0f, 1.0f);
    } while (squared_length(p) >= 1.0);
    return p;
}

vec3 random_unit_vector(inout uint state) {
    return normalize(random_in_unit_sphere2(state));
}

vec3 random_on_hemisphere(vec3 normal, inout uint state) {
    vec3 on_unit_sphere = random_unit_vector(state);
    return on_unit_sphere;

    // THIS IS WRONG, SOMEHOW
    //if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    //return on_unit_sphere;
    //else
    //return -on_unit_sphere;
}

vec3 origin(Ray r)
{
    return r.A;
}

vec3 direction(Ray r)
{
    return r.B;
}

vec3 point_at_parameter(Ray r, float t)
{
    return r.A + t*r.B;
}

Ray get_ray(Camera cam, float s, float t, inout uint state)
{
    vec3 rd = cam.lens_radius * random_in_unit_sphere2(state);
    vec3 offset = cam.u * rd.x + cam.v * rd.y;
    return Ray(
        cam.origin + offset,
        cam.lower_left_corner + s * cam.horizontal + t * cam.vertical - cam.origin - offset);
}

bool lambertian_texture(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 target = rec.p + rec.normal + random_in_unit_sphere2(state);
    scattered = Ray(rec.p, target - rec.p);
    attenuation = ctex[rec.texture_index].color;
    return true;
}

bool lambertian_material(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 target = rec.p + rec.normal + random_in_unit_sphere2(state);
    scattered = Ray(rec.p, target - rec.p);
    attenuation = material_albedo[rec.material_index];
    return true;
}

bool metal_material(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 reflected = reflect( normalize(direction(r)), rec.normal );
    scattered = Ray(rec.p, reflected + material_fuzz[rec.material_index]*random_in_unit_sphere2(state));
    attenuation = material_albedo[rec.material_index];
    return ( dot( direction(scattered), rec.normal ) > 0 );
}

bool _refract(vec3 v, vec3 n, float ni_over_nt, inout vec3 refracted)
{
    vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    return false;
}

bool dielectric_simple(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 outward_normal;
    vec3 reflected = reflect( direction(r), rec.normal );
    float ni_over_nt;
    attenuation = vec3(1.0f, 1.0f, 1.0f);
    vec3 refracted = vec3(0.0f, 0.0f, 0.0f);
    float ref_idx = material_ior[rec.material_index];

    if (dot(direction(r), rec.normal) > 0) {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx;
    } else {
        outward_normal = rec.normal;
        ni_over_nt = 1.0f / ref_idx;
    }

    if (_refract(direction(r), outward_normal, ni_over_nt, refracted)) {
        scattered = Ray(rec.p, refracted);
    } else {
        scattered = Ray(rec.p, reflected);
    }
    return true;
}

bool dielectric(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 outward_normal;
    vec3 reflected = reflect( direction(r), rec.normal );
    float ni_over_nt;
    attenuation = vec3(1.0f, 1.0f, 1.0f);
    vec3 refracted;
    //float ref_idx = 0.6;
    float ref_idx = material_ior[rec.material_index];
    float reflect_prob;
    float cosine;

    if (dot(direction(r), rec.normal) > 0.0f) {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx;
        cosine = ref_idx * dot(direction(r), rec.normal) / length(direction(r));
    } else {
        outward_normal = rec.normal;
        ni_over_nt = 1.0f / ref_idx;
        cosine = -dot(direction(r), rec.normal) / length(direction(r));
    }

    if (_refract(direction(r), outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine, ref_idx);
    } else {
        reflect_prob = 1.0f;
    }

    if (random_float(state) < reflect_prob) {
        scattered = Ray(rec.p, reflected);
    } else {
        scattered = Ray(rec.p, refracted);
    }
    return true;
}

bool hit_sphere(Ray r, float t_min, float t_max, int object_index, inout hit_record rec)
{
    vec3 center = sphere2[object_index].center;
    float radius = sphere2[object_index].radius;

    vec3 oc = origin(r) - center;
    float a = dot(direction(r), direction(r));
    float b = 2.0 * dot(oc, direction(r));
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / (2*a);
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.material_type = sphere2[object_index].material_type;
            rec.material_index = sphere2[object_index].material_index;
            rec.texture_index = sphere2[object_index].texture_index;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2*a);
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.material_type = sphere2[object_index].material_type;
            rec.material_index = sphere2[object_index].material_index;
            rec.texture_index = sphere2[object_index].texture_index;
            return true;
        }
    }
    return false;
}

bool hittable_list_hit(Ray r, float t_min, float t_max, inout hit_record rec)
{
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = MAX_FLOAT;
    for (int i = 0; i < NUM_SPHERES; i++) {
        if (hit_sphere(r, t_min, closest_so_far, i, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

vec3 color(Ray r, inout uint state, inout vec2 state2)
{
    Ray cur_ray = r;
    vec3 cur_attenuation = vec3(1.0f);
    vec2 f = vec2(1.0);
    for(int i = 0; i < 50; i++) {
        hit_record rec;
        if ( hittable_list_hit(cur_ray, 0.001f, MAX_FLOAT, rec) ) {
            Ray scattered;
            vec3 attenuation = vec3(0.0f);
            if (rec.material_type == LAMBERTIAN) {
                if(lambertian_material(cur_ray, rec, attenuation, scattered, state)) {
                    cur_attenuation *= attenuation;
                    cur_ray = scattered;
                } else {
                    return vec3(0.0,0.0,0.0);
                }
            } else if (rec.material_type == METAL) {
                if (metal_material(cur_ray, rec, attenuation, scattered, state)) {
                    cur_attenuation *= attenuation;
                    cur_ray = scattered;
                } else {
                    return vec3(0.0,0.0,0.0);
                }

            } else if (rec.material_type == DIELECTRIC) {
                if (dielectric(cur_ray, rec, attenuation, scattered, state)) {
                    cur_attenuation *= attenuation;
                    cur_ray = scattered;
                } else {
                    return vec3(0.0,0.0,0.0);
                }

            } else if (rec.material_type == SOLID_TEXTURE) {
                if(lambertian_texture(cur_ray, rec, attenuation, scattered, state)) {
                    cur_attenuation *= attenuation;
                    cur_ray = scattered;
                } else {
                    return vec3(0.0,0.0,0.0);
                }
            }
        } else {
            vec3 unit_direction = normalize(direction(cur_ray));
            float t = 0.5f*(unit_direction.y + 1.0f);
            vec3 c = (1.0f-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
            return cur_attenuation * c;
        }
    }
    return vec3(0.0f, 0.0f, 0.0f);
}

void main() {
    vec2 st = gl_FragCoord.xy/props.xy;
    uint state = uint(gl_FragCoord.x) * 1973u + uint(gl_FragCoord.y) * 9277u + uint(frame_number) * 26699u;

    g_seed = random(gl_FragCoord.xy * (mod(time, 100.)));
    if(isnan(g_seed)) {
        g_seed = 0.25;
    }

    vec3 col = vec3(0.0f);

#define ns 1
    for (int i = 0; i < ns; i++) {
        float u = float(gl_FragCoord.x + RandomValue(state)) / float(props.x);
        float v = float(gl_FragCoord.y + RandomValue(state)) / float(props.y);

        Ray r = get_ray(cam, u, v, state);
        col += color(r, state, st);
    }

    // Gamma2 as rt1w
    //col = col / ns;
    //col  = sqrt(col);
    //FragColor = vec4(col, 1.0f);

    // Gamma as in learnopengl
    //float gamma = 1.6;
    //col = col / ns;
    //col.rgb = pow(col.rgb, vec3(1.0/gamma));
    //FragColor = vec4(col, 1.0f);

    //FragColor = vec4(col / float(ns), 1.0f);  // This is only color div by number of samples
    //FragColor = texture(tex, ftexcoord);
    col = col / ns;
    //col = lottes(col);

/**
    vec3 exposed = col * 0.8; // Try values like 0.1 to 0.5
    vec3 toneMapped = exposed / (exposed + vec3(1.0));
    vec3 final = linearToSRGB(toneMapped);
*/

    //col = linearToSRGB(col);
    col = pow(col, vec3(1.0/1.8)); // Lower the gamma some, i like the better even if it's wrong

    //FragColor = vec4(aces(col), 1.0f) * .999f;
    FragColor = vec4(col, 1.0f);
}
