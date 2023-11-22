#version 410

#define FLT_MAX 3.402823466e+38

struct Camera {
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
};

struct Sphere {
    vec3 center;
    float radius;
};

uniform sampler2D tex; // texture uniform
uniform vec2 props;
uniform int NUM_SPHERES;
uniform Camera cam;
uniform Sphere sphere[2];

in vec2 ftexcoord;
in vec4 gl_FragCoord;

layout(location = 0) out vec4 FragColor;

struct Ray {
    vec3 A;
    vec3 B;
};

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    int index;
};

float random(vec2 st)
{
    //https://stackoverflow.com/questions/53500550/generating-a-pseudo-random-number-between-0-an-1-glsl-es
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
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

vec3 random_in_unit_sphere(inout uint state)
{
    vec3 p;
    do {
        p = 2.0f * vec3(RandomValue(state), RandomValue(state), RandomValue(state)) - vec3(1.0f);
    } while (length(p) >= 1.0);
    return p;
}

bool hit_sphere(Ray r, float t_min, float t_max, inout hit_record rec)
{
    vec3 center = sphere[rec.index].center;
    float radius = sphere[rec.index].radius;

    vec3 oc = origin(r) - center;
    float a = dot(direction(r), direction(r));
    float b = 2.0 * dot(oc, direction(r));
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant > 0)
    {
        float temp = (-b - sqrt(b*b - a*c)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = point_at_parameter(r, rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
        temp = (-b + sqrt(b*b - a*c)) / a;
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
    float closest_so_far = FLT_MAX;
    for (int i = 0; i < NUM_SPHERES; i++)
    {
        temp_rec.index = i;
        if (hit_sphere(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

vec3 color(Ray r, inout uint state)
{
    Ray cur_ray = r;
    float cur_attenuation = 1.0f;
    for(int i = 0; i < 50; i++)
    {
        hit_record rec;
        if ( hittable_list_hit(r, 0.001f, FLT_MAX, rec) )
        {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere(state);
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
//    if ( hittable_list_hit(r, 0.0f, FLT_MAX, rec) )
//    {
//        return 0.5 * vec3(rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1);
//    }
//    else
//    {
//        vec3 unit_direction = normalize(direction(r));
//        float t = 0.5f * (unit_direction.y + 1.0f);
//        return (1.0f - t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
//    }
    return vec3(0.0,0.0,0.0);
}

void main() {
    vec2 st = gl_FragCoord.xy/props.xy;
    uint pixelIndex = int(gl_FragCoord.y * props.x + gl_FragCoord.x);
    float u = float(gl_FragCoord.x + RandomValue(pixelIndex)) / float(props.x);
    float v = float(gl_FragCoord.y + RandomValue(pixelIndex)) / float(props.y);

    Ray r = get_ray(cam, u, v);
    //Ray r = Ray(cam.origin, cam.lower_left_corner + u*cam.horizontal + v*cam.vertical);
    FragColor = vec4( color(r, pixelIndex), 1.0);
    //FragColor = texture(tex, ftexcoord);
}
