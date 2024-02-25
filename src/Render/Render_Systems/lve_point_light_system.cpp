#include "lve_point_light_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
namespace lve
{

    PointLightSystem::PointLightSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout global_set_layout) : lveDevice{device}
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout global_set_layout)
    {
        // VkPushConstantRange pushConstantRange{};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptor_set_layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }
    void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
    {

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.attribute_descriptions.clear();
        pipelineConfig.bind_descriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(lveDevice, pipelineConfig, "/home/bios/CLionProjects/2DEngine/Resources/Shaders/point_light.vert.spv", "/home/bios/CLionProjects/2DEngine/Resources/Shaders/point_light.frag.spv");
    }

    void PointLightSystem::render(FrameInfo &frame_info)
    {
        lvePipeline->Bind(frame_info.command_buffer);
        vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frame_info.global_descriptor_set, 0, nullptr);

        vkCmdDraw(frame_info.command_buffer, 6, 1, 0, 0);
    }
} // namespace lve
