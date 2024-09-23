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
#include <map>

namespace VectorVertex
{
    class PointLightSystem
    {
    public:
        PointLightSystem(VVDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout global_set_layout);
        PointLightSystem(VVDevice &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> global_set_layout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;
        void Update(FrameInfo &frame_info, SceneRenderInfo &scene_info, GlobalUBO &ubo);

        void render(FrameInfo &frame_info, SceneRenderInfo &scene_info);

    private:
        void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout);
        void CreatePipelineLayout(VkDescriptorSetLayout global_set_layout);
        void CreatePipeline(VkRenderPass renderPass);

        VVDevice &vvDevice;

        std::unique_ptr<VVPipeline> pipeline;

        VkPipelineLayout pipelineLayout;
    };
} // namespace VectorVertex
