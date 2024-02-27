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
    mat4 inv_view;
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
    vec3 specular_light = vec3(0.0);
    vec3 surface_normal = normalize(fragNormalWorld);

    vec3 camera_pos_world = ubo.inv_view[3].xyz;
    vec3 view_direction = normalize(camera_pos_world - fragPosWorld);

    for(int i= 0; i < ubo.num_lights; i++){
        PointLight light = ubo.point_lights[i];
        vec3 direction_to_light = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(direction_to_light, direction_to_light); // distance ^ 2
        direction_to_light = normalize(direction_to_light);

        float cos_ang_incidence = max(dot(surface_normal, direction_to_light), 0.0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuse_light += intensity * cos_ang_incidence;

        //specular lightning
        vec3 half_angle = normalize(direction_to_light + view_direction);
        float blinn_term = dot(surface_normal , half_angle);
        blinn_term = clamp(blinn_term, 0, 1);
        blinn_term = pow(blinn_term, 512.0); // higher values -> sharp highlight

        specular_light += intensity * blinn_term;
    }

    outColor = vec4(diffuse_light * fragColor + specular_light * fragColor, 1.0);

}