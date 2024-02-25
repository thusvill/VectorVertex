#version 450

layout (location =0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;


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

    vec3 direction_to_light = ubo.light_position - fragPosWorld;
    float attenuation = 1.0 / dot(direction_to_light, direction_to_light); // distance ^ 2

    vec3 light_color = ubo.light_color.xyz * ubo.light_color.w * attenuation;
    vec3 ambient_light = ubo.ambient_color.xyz * ubo.ambient_color.w;
    vec3 diffuse_light = light_color * max(dot(normalize(fragNormalWorld), normalize(direction_to_light)), 0.0);


    outColor = vec4((diffuse_light + ambient_light) * fragColor, 1.0);

}