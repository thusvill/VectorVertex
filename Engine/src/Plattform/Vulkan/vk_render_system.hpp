#pragma once
#include <vulkan/vulkan.h>
#include <Core/vvpch.hpp>
#include <vk_descriptors.hpp>
#include <vk_pipeline.hpp>
#include <Material.hpp>
#include <vk_frame_info.hpp>
#include <FrameBuffer.hpp>

namespace VectorVertex
{
    class Entity;
    struct PushConstantData
    {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.f};
        MaterialPushConstant materialData;
    };
    struct PointLightPushConstantData
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius{};
    };
    class VulkanRenderSystem
    {
    public:
        VulkanRenderSystem() = default;
        ~VulkanRenderSystem() {}

        virtual void Update(std::unordered_map<UUID, Entity> objects, FrameInfo &info) = 0;
        virtual void Render(std::unordered_map<UUID, Entity> objects, FrameInfo &info) = 0;

    protected:
        virtual void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout) = 0;
        void CreatePipeline(const std::string vertex_shader, const std::string fragment_shader);
        void CreatePipeline(FrameBuffer &framebuffer, const std::string vertex_shader, const std::string fragment_shader);

        Scope<VKPipeline> pipeline;

        VkPipelineLayout pipelineLayout;
    };
}