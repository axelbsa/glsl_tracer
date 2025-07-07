#version 410

#define MAX_FLOAT 1e5
#define FLT_MAX 3.402823466e+38
#define PHI 1.61803398874989484820459
#define PI 3.1415926535897932385
#define TAU 2. * PI


struct Ray {
    vec3 A;
    vec3 B;
};

struct Camera {
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
};

struct Sphere {
    vec3 center;
    float radius;
    int material_index;
};

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    int index;
};

uniform float time;
uniform sampler2D tex; // texture uniform
uniform vec2 props;
uniform int NUM_SPHERES;
uniform Camera cam;
uniform Sphere sphere[2];

in vec2 ftexcoord;
in vec4 gl_FragCoord;

layout(location = 0) out vec4 FragColor;

float g_seed = 0.25;

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
};



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

float RandomValue(inout uint state)
{
    state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28 ) + 4)) ^ state ) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0f;
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

Ray get_ray(Camera cam, float u, float v)
{
    return Ray(cam.origin, cam.lower_left_corner + u*cam.horizontal + v*cam.vertical - cam.origin);
}

float squared_length(vec3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
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

bool lambertian_scatter(
inout Ray r_in,
inout hit_record rec,
inout vec3 attenuation, inout Ray scattered)
{
/**    vec3 target = rec.p + rec.normal + random_in_unit_sphere2(state);
    scattered = Ray(rec.p, target-rec.p);*/
    return true;
}

bool lambertian_material(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 target = rec.p + rec.normal + random_in_unit_sphere2(state);
    scattered = Ray(rec.p, target - rec.p);
    attenuation = vec3(0.7, 0.4, 0.3);
    return true;
}

bool metal_material(Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
    vec3 reflected = reflect( normalize(direction(r)), rec.normal );
    scattered = Ray(rec.p, reflected);
    attenuation = vec3(0.6, 0.2, 0.3);
    return ( dot( direction(scattered), rec.normal ) > 0 );
}

bool hit_sphere(Ray r, float t_min, float t_max, int object_index, inout hit_record rec)
{
    vec3 center = sphere[object_index].center;
    float radius = sphere[object_index].radius;

    vec3 oc = origin(r) - center;
    float a = dot(direction(r), direction(r));
    float b = 2.0 * dot(oc, direction(r));
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant)) / (2*a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2*a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p - center) / radius;
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
    for (int i = 0; i < NUM_SPHERES; i++)
    {
        if (hit_sphere(r, t_min, closest_so_far, i, temp_rec))
        {
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
    float cur_attenuation = 1.0f;
    vec2 f = vec2(1.0);
    for(int i = 0; i < 50; i++)
    {
        hit_record rec;
        if ( hittable_list_hit(cur_ray, 0.001f, MAX_FLOAT, rec) )
        {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere2(state);
            //vec3 direction = random_on_hemisphere(rec.normal, state);
            cur_attenuation *= 0.5f;
            cur_ray = Ray(rec.p, target-rec.p);
        }
        else
        {
            vec3 unit_direction = normalize(direction(cur_ray));
            float t = 0.5f*(unit_direction.y + 1.0f);
            vec3 c = (1.0f-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
            return cur_attenuation * c;
        }
    }
    return vec3(0.0,0.0,0.0);
}

void main() {
    vec2 st = gl_FragCoord.xy/props.xy;
    uint pixelIndex = int(gl_FragCoord.y * props.x + gl_FragCoord.x);

    g_seed = random(gl_FragCoord.xy * (mod(time, 100.)));
    if(isnan(g_seed)){
        g_seed = 0.25;
    }

    vec3 col = vec3(1.0f);

#define ns 40
    for (int i = 0; i < ns; i++)
    {
        float u = float(gl_FragCoord.x + RandomValue(pixelIndex)) / float(props.x);
        float v = float(gl_FragCoord.y + RandomValue(pixelIndex)) / float(props.y);

        Ray r = get_ray(cam, u, v);
        col += color(r, pixelIndex, st);
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
    //col = vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));
    FragColor = vec4(col, 1.0f) * .999;
}
