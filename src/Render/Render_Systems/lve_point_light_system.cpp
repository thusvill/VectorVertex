#include "lve_point_light_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
namespace lve
{
    struct PointLightPushConstants
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

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
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptor_set_layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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
        lvePipeline = std::make_unique<LvePipeline>(lveDevice, pipelineConfig, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/point_light.vert.spv", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/point_light.frag.spv");
    }

    void PointLightSystem::Update(FrameInfo &frame_info, GlobalUBO &ubo)
    {
        auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            frame_info.frame_time,
            {0.f, -1.f, 0.f});

        int light_index = 0;
        for (auto &kv : frame_info.game_objects)
        {
            auto &_obj = kv.second;
            if (_obj.point_light == nullptr)
                continue;

            assert(light_index < MAX_LIGHTS && "Point light exceeds maximum number of lights!");

                _obj.transform.translation = glm::vec3(rotateLight * glm::vec4(_obj.transform.translation, 1.0f));
            ubo.point_lights[light_index].position = glm::vec4(_obj.transform.translation, 1.0f);
            ubo.point_lights[light_index].color = glm::vec4(_obj.color, _obj.point_light->light_intensity);
            light_index += 1;
        }
        ubo.num_lights = light_index;
    }

    void PointLightSystem::render(FrameInfo &frame_info)
    {
        lvePipeline->Bind(frame_info.command_buffer);
        vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frame_info.global_descriptor_set, 0, nullptr);

        for (auto &kv : frame_info.game_objects)
        {
            auto &_obj = kv.second;
            if (_obj.point_light == nullptr)
                continue;

            PointLightPushConstants push{};
            push.position = glm::vec4(_obj.transform.translation, 1.0f);
            push.color = glm::vec4(_obj.color, _obj.point_light->light_intensity);
            push.radius = _obj.transform.scale.x;

            vkCmdPushConstants(frame_info.command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);
            vkCmdDraw(frame_info.command_buffer, 6, 1, 0, 0);
        }
    }
} // namespace lve
