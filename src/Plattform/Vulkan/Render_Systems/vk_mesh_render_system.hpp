#pragma once
#include <vvpch.hpp>
#include <vk_render_system.hpp>

namespace VectorVertex
{
    class VulkanMeshRenderer : public VulkanRenderSystem
    {
        public:
            VulkanMeshRenderer() = default;
            VulkanMeshRenderer(std::vector<VkDescriptorSetLayout> layouts);
            VulkanMeshRenderer(std::vector<VkDescriptorSetLayout> layouts, std::string vertex_shader, std::string fragment_shader);

            virtual void Update(std::unordered_map<UUID, Entity> objects, FrameInfo& info) override; //does nothing yet
            virtual void Render(std::unordered_map<UUID, Entity> objects, FrameInfo& info) override;

        private:
        virtual void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout) override;
    };
} // namespace VectorVertex
