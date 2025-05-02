#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUno
{
    mat4 projectionViewMatrix;
    vec3 directionToLight;
} globalUno;

void main()
{
    outColor = vec4(fragColor, 1.0);
}