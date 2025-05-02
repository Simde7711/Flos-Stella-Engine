#version 450 

const vec2 OFFSETS[6] = vec2[]
(
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

layout(set = 0, binding = 0) uniform GlobalUno
{
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    vec3 lightPosition; 
    vec4 lightColor;
} globalUno;

const float LIGHT_RADIUS = 0.05;

void main()
{
    fragOffset = OFFSETS[gl_VertexIndex];

    vec4 lightInCameraSpace = globalUno.view * vec4(globalUno.lightPosition, 1.0);
    vec4 positionInCameraSpace = lightInCameraSpace + LIGHT_RADIUS * vec4(fragOffset, 0.0, 0.0);
    gl_Position = globalUno.projection * positionInCameraSpace;
}