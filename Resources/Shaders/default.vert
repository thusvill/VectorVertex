#version 450

layout(location = 0 ) in vec3 position;
layout(location = 1 ) in vec3 color;
layout(location = 2 ) in vec3 normal;
layout(location = 3 ) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

layout(set = 0, binding =0) uniform globalUbo{
    mat4 projection;
    mat4 view;
    vec4 ambient_color; // w is intensity
    vec3 light_position;
    vec4 light_color;
} ubo;

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;



void main(){
    vec4 position_world = push.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projection * ubo.view *  position_world;

    fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
    fragPosWorld = position_world.xyz;

    fragColor = color;
}