#pragma once

#include "../vk_pipeline.hpp"
#include "../vk_camera.hpp"
#include "../vk_frame_info.hpp"
#include "../vk_descriptors.hpp"
#include "../vk_texture.hpp"

#include <imgui.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vvpch.hpp>

namespace VectorVertex
{
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.f};
        MaterialPushConstant materialData;
    };
    class LveRenderSystem
    {
    public:
        LveRenderSystem(VkDescriptorSetLayout global_set_layout);
        LveRenderSystem(std::vector<VkDescriptorSetLayout> global_set_layout);
        ~LveRenderSystem();

        LveRenderSystem(const LveRenderSystem &) = delete;
        LveRenderSystem &operator=(const LveRenderSystem &) = delete;
        void renderGameobjects(FrameInfo &frame_info, SceneRenderInfo &scene_info);
        void renderImGui(VkCommandBuffer commandBuffer);
        

    private:
        void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout);
        void CreatePipelineLayout(VkDescriptorSetLayout des_set_layout);
        void CreatePipeline();

        std::unique_ptr<VKPipeline> pipeline;

        VkPipelineLayout pipelineLayout;
       // VkDescriptorSet texture_descriptorset;
        //std::unique_ptr<LveDescriptorSetLayout> texture_layout;
    };
} // namespace VectorVertex
