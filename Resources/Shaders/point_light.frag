#version 450

layout (location =0) in vec2 frag_offset;
layout (location =0) out vec4 outColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding =0) uniform globalUbo {
    mat4 projection;
    mat4 view;
    mat4 inv_view;
    vec4 ambient_color; // w is intensity
    PointLight point_lights[10];
    int num_lights;
} ubo;

layout(set=1, binding=0) uniform sampler2D material_texture;

layout(push_constant) uniform Push {
    vec4 position;
    vec4 color;
    float radius;
}push;

const float M_PI = 3.14159265358979323846;
void main() {
    float dis = sqrt(dot(frag_offset, frag_offset));
    if(dis >= 1.0) discard;
    
    float cosDis = 0.5 * (cos(dis * M_PI) + 1.0);
    //outColor = vec4(push.color.xyz + cosDis, cosDis);
}