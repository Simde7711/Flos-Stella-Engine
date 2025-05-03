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

struct PointLight 
{
    vec4 position; // ignore w
    vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUno
{
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} globalUno;

layout(push_constant) uniform Push
{
    vec4 position;
    vec4 color;
    float radius;
} push;


void main()
{
    fragOffset = OFFSETS[gl_VertexIndex];
    vec3 cameraRightWorld = {globalUno.view[0][0], globalUno.view[1][0], globalUno.view[2][0]};
    vec3 cameraUpWorld = {globalUno.view[0][1], globalUno.view[1][1], globalUno.view[2][1]};

    vec3 positionWorld = push.position.xyz
    + push.radius * fragOffset.x * cameraRightWorld
    + push.radius * fragOffset.y * cameraUpWorld;

  gl_Position = globalUno.projection * globalUno.view * vec4(positionWorld, 1.0);
}