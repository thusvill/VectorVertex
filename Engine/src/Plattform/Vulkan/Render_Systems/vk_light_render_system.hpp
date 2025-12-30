#pragma once
#include <Core/vvpch.hpp>
#include <vk_render_system.hpp>
#include <FrameBuffer.hpp>
namespace VectorVertex
{
    class VulkanLightRenderer : public VulkanRenderSystem
    {
    public:
        VulkanLightRenderer() = default;
        VulkanLightRenderer(std::vector<VkDescriptorSetLayout> layouts);
        VulkanLightRenderer(std::vector<VkDescriptorSetLayout> layouts, FrameBuffer &framebuffer);
        VulkanLightRenderer(std::vector<VkDescriptorSetLayout> layouts, std::string vertex_shader, std::string fragment_shader);

        virtual void Update(std::unordered_map<UUID, Entity> objects, FrameInfo &info) override;
        virtual void Render(std::unordered_map<UUID, Entity> objects, FrameInfo &info) override;

        Entity *m_Camera;

    private:
        virtual void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout) override;
    };
} // namespace VectorVertex