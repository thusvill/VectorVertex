#version 450

layout (location =0) in vec2 frag_offset;
layout (location =0) out vec4 outColor;

layout(set = 0, binding =0) uniform globalUbo{
    mat4 projection;
    mat4 view;
    vec4 ambient_color; // w is intensity
    vec3 light_position;
    vec4 light_color;
} ubo;

void main(){
    float dis = sqrt(dot(frag_offset, frag_offset));
    if(dis >1.0) discard;
    outColor = vec4(ubo.light_color.xyz, 1.0);
}