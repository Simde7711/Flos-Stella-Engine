#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

struct PointLight 
{
    vec4 position; // ignore w
    vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUno
{
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} globalUno;

layout(push_constant) uniform Push
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main()
{
    vec3 diffuseLight = globalUno.ambientLightColor.xyz * globalUno.ambientLightColor.w;
    vec3 surfaceNormal = normalize(fragNormalWorld);

    for (int i = 0; i < globalUno.numLights; i++)
    {
        PointLight light = globalUno.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;
    }

    outColor = vec4((diffuseLight * fragColor) * fragColor, 1.0);
}