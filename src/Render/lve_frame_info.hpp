#pragma once

#include "lve_camera.hpp"
#include "lve_game_object.hpp"

#include <vulkan/vulkan.h>

namespace lve
{
#define MAX_LIGHTS 10

    struct PointLight
    {
        glm::vec4 position{};
        glm::vec4 color{}; // w = intensity
    };
    struct GlobalUBO
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::vec4 ambient_color{1.f, 1.f, 1.f, .02f};
        PointLight point_lights[MAX_LIGHTS];
        int num_lights;
    };
    struct FrameInfo
    {
        int frame_index;
        float frame_time;
        VkCommandBuffer command_buffer;
        LveCamera &camera;
        VkDescriptorSet global_descriptor_set;
        LveGameObject::Map &game_objects;
    };
} // namespace lve
