#pragma once
#include <vulkan/vulkan.h>
#include <vvpch.hpp>
#include <vk_descriptors.hpp>
#include <vk_pipeline.hpp>
#include <Material.hpp>
#include <vk_frame_info.hpp>

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
        VulkanRenderSystem(std::string vertex_shader, std::string fragment_shader);
        VulkanRenderSystem(VkDescriptorSetLayout layout, std::string vertex_shader, std::string fragment_shader);
        VulkanRenderSystem(std::vector<VkDescriptorSetLayout> additional_layouts, std::string vertex_shader, std::string fragment_shader);
        VulkanRenderSystem(std::vector<VkDescriptorSetLayout> additional_layouts, uint32_t push_constant_size, std::string vertex_shader, std::string fragment_shader);
        ~VulkanRenderSystem() {}

        void UpdateLights(std::unordered_map<UUID, Entity> objects, GlobalUBO ubo);
        void RenderLights(std::unordered_map<UUID, Entity> objects, Entity Camera);
        void Bind(Entity object, FrameInfo info);
        void BindLight(Entity object, Entity Camera);

    private:
        void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout);
        void CreatePipelineLayout(VkDescriptorSetLayout des_set_layout);
        void CreatePipeline(const std::string vertex_shader, const std::string fragment_shader);

        Scope<VKPipeline> pipeline;

        VkPipelineLayout pipelineLayout;

        void UploadShaderData(Entity object, FrameInfo info);
        void UploadLightData(Entity object, Entity Camera);
    };
}