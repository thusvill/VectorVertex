#pragma once

#include "../vv_pipeline.hpp"
#include "../vv_device.hpp"
#include "../vv_camera.hpp"
#include "../vv_game_object.hpp"
#include "../vv_frame_info.hpp"


#include <imgui.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <array>

namespace VectorVertex
{
    class LveRenderSystem
    {
    public:
        LveRenderSystem(VVDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout global_set_layout);
        ~LveRenderSystem();

        LveRenderSystem(const LveRenderSystem &) = delete;
        LveRenderSystem &operator=(const LveRenderSystem &) = delete;
        void renderGameobjects(FrameInfo &frame_info);
        void renderImGui(VkCommandBuffer commandBuffer);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout global_set_layout);
        void CreatePipeline(VkRenderPass renderPass);

        VVDevice &vvDevice;

        std::unique_ptr<VVPipeline> pipeline;

        VkPipelineLayout pipelineLayout;
    };
} // namespace VectorVertex
