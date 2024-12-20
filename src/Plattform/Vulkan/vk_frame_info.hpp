#pragma once
#include <vvpch.hpp>
#include "vk_camera.hpp"
#include "vk_descriptors.hpp"
#include "vk_renderer.hpp"
#include <imgui.h>
#include <UUID.hpp>
#include <vulkan/vulkan.h>
//#include <Entity.hpp>

namespace VectorVertex
{
    class Entity;
    #define MAX_LIGHTS 10

    struct PointLight
    {
        glm::vec4 position{};
        glm::vec4 color{}; // w = intensity
    };
    struct GlobalUBO
    {
        GlobalUBO() = default;
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverse_view_matrix{1.f};
        glm::vec4 ambient_color{1.f, 1.f, 1.f, .02f};
        PointLight point_lights[MAX_LIGHTS];
        int num_lights;
    };

    struct FrameInfo
    {
        int frame_index = 0;
        float frame_time = 0.0f;
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        GlobalUBO& ubo;
        
    };

    struct SceneRenderInfo
    {

        std::unordered_map<UUID, Entity> &entities;
        Entity &m_Camera;
        std::unordered_map<int, VkDescriptorSet> descriptor_sets = {};
        std::vector<std::reference_wrapper<VKDescriptorPool>> &global_pool;
    };
} // namespace VectorVertex
