#version 450

layout (location =0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;


struct PointLight{
    vec4 position;
    vec4 color;
};

layout(set = 0, binding =0) uniform globalUbo{
    mat4 projection;
    mat4 view;
    vec4 ambient_color; // w is intensity
    PointLight point_lights[10];
    int num_lights;
} ubo;

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main(){

    vec3 diffuse_light = ubo.ambient_color.xyz * ubo.ambient_color.w;
    vec3 surface_normal = normalize(fragNormalWorld);

    for(int i= 0; i < ubo.num_lights; i++){
        PointLight light = ubo.point_lights[i];
        vec3 direction_to_light = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(direction_to_light, direction_to_light); // distance ^ 2
        float cos_ang_incidence = max(dot(surface_normal, normalize(direction_to_light)), 0.0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuse_light += intensity * cos_ang_incidence;
    }

    outColor = vec4(diffuse_light * fragColor, 1.0);

}