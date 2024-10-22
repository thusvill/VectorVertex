#pragma once
#include <vulkan/vulkan.h>
#include <vvpch.hpp>
#include <vk_descriptors.hpp>
#include <vk_pipeline.hpp>
#include <Material.hpp>


namespace VectorVertex
{
    class Entity;
    struct PushConstantData
    {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.f};
        MaterialPushConstant materialData;
    };
    class VulkanRenderSystem
    {
    public:
    VulkanRenderSystem() = default;
        VulkanRenderSystem(std::string vertex_shader, std::string fragment_shader);
        VulkanRenderSystem(std::vector<VkDescriptorSetLayout> additional_layouts,std::string vertex_shader, std::string fragment_shader);
        ~VulkanRenderSystem(){}
        void Bind(Entity object);

    private:
        void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout);
        void CreatePipelineLayout(VkDescriptorSetLayout des_set_layout);
        void CreatePipeline(const std::string vertex_shader, const std::string fragment_shader);

        Scope<VKPipeline> pipeline;

        VkPipelineLayout pipelineLayout;

        void UploadShaderData(Entity object);
    };
}