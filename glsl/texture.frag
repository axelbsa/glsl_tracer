#version 330 core
out vec4 FragColor;
vec4 colors;

in vec2 TexCoords;

uniform sampler2D currentFrame;
uniform sampler2D previousFrame;
uniform int frameCount;

vec3 accumulated;
vec3 current = texture(currentFrame, TexCoords.st).rgb;
vec3 previous = texture(previousFrame, TexCoords.st).rgb;

// This needs to match ns in the other shader
// Only adjust if needed really
#define ns 3

void main()
{
    if (frameCount == 1) {
        // First frame: just use the current sample
        accumulated = current;
    } else {
        accumulated = (previous * float((frameCount - 1) * ns) + current) / float(frameCount * ns);
    }
    FragColor = vec4(accumulated, 1.0f);

//    float weight = 1.0 / float(frameCount);
//    current = texture(screenTexture, TexCoords);
//    accumulated = texture(accumulated, TexCoords);
//    accumulated = mix(accumulateTexture, screenTexture, weight);
    //FragColor = vec4(colors.r, colors.g, colors.b, 1.0f) ;
}
