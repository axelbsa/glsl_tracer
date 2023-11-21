#version 410

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
uniform Camera cam;
uniform Sphere sphere[2];

in vec2 ftexcoord;
in vec4 gl_FragCoord;

layout(location = 0) out vec4 FragColor;

struct Ray {
    vec3 A;
    vec3 B;
};

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

float hit_sphere(Ray r, vec3 center, float radius)
{
    vec3 oc = origin(r) - center;
    float a = dot(direction(r), direction(r));
    float b = 2.0 * dot(oc, direction(r));
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0)
    {
        return -1.0f;
    }
    else
    {
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }
}

vec3 color(Ray r)
{
    float t = hit_sphere(r, vec3(0.0f, 0.0f, -1.0f), 0.5f);
    if (t > 0.0)
    {
        vec3 N = normalize(point_at_parameter(r, t) - vec3(0.0f, 0.0f, -1.0f));
        return 0.5 * vec3(N.x + 1, N.y + 1, N.z + 1);
    }
    vec3 unit_direction = normalize(direction(r));
    t = 0.5 * (unit_direction.y + 1.0f);
    return (1.0 - t) * vec3(1.0) + t * vec3(0.5, 0.7, 1.0);
}

void main() {
    float u = float(gl_FragCoord.x) / float(props.x);
    float v = float(gl_FragCoord.y) / float(props.y);

    Ray r = Ray(cam.origin, cam.lower_left_corner + u*cam.horizontal + v*cam.vertical);
    FragColor = vec4( color(r), 1.0);
    //FragColor = texture(tex, ftexcoord);
}
