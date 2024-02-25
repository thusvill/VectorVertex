#pragma once

#include "lve_camera.hpp"
#include "lve_game_object.hpp"

#include <vulkan/vulkan.h>

namespace lve
{
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
