#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUno
{
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition; 
    vec4 lightColor;
} globalUno;

layout(push_constant) uniform Push
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main()
{
    vec3 directionToLight = globalUno.lightPosition - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);

    vec3 lightColor = globalUno.lightColor.xyz * globalUno.lightColor.w * attenuation;
    vec3 ambientLight = globalUno.ambientLightColor.xyz * globalUno.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

    outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}