#pragma once

#include "vv_camera.hpp"
#include "vv_game_object.hpp"
#include "vv_descriptors.hpp"
#include "vv_renderer.hpp"
#include <imgui.h>
#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

namespace VectorVertex
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
        glm::mat4 inverse_view_matrix{1.f};
        glm::vec4 ambient_color{1.f, 1.f, 1.f, .02f};
        PointLight point_lights[MAX_LIGHTS];
        int num_lights;
    };

    struct FrameInfo
    {
        int frame_index;
        float frame_time;
        VkCommandBuffer command_buffer;
        VVCamera &camera;
        std::unordered_map<int,VkDescriptorSet> descriptor_sets;
        VVGameObject::Map &game_objects;
        std::vector<std::reference_wrapper<VVDescriptorPool>> &global_pool;
        VVRenderer &renderer;
        ImTextureID frame_image;
    };
} // namespace VectorVertex
