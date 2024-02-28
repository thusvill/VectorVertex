#pragma once

#include "../vv_pipeline.hpp"
#include "../vv_device.hpp"
#include "../vv_camera.hpp"
#include "../vv_game_object.hpp"
#include "../vv_frame_info.hpp"

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
        LveRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout global_set_layout);
        ~LveRenderSystem();

        LveRenderSystem(const LveRenderSystem &) = delete;
        LveRenderSystem &operator=(const LveRenderSystem &) = delete;
        void renderGameobjects(FrameInfo &frame_info);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout global_set_layout);
        void CreatePipeline(VkRenderPass renderPass);

        LveDevice &lveDevice;

        std::unique_ptr<LvePipeline> lvePipeline;

        VkPipelineLayout pipelineLayout;
    };
} // namespace VectorVertex
