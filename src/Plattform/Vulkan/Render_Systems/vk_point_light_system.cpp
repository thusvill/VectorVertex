#include "vk_point_light_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <Components.hpp>
#include <Entity.hpp>
#include <Renderer.hpp>
#include <Application.hpp>

namespace VectorVertex
{
    struct PointLightPushConstants
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius{};
    };

    PointLightSystem::PointLightSystem(VkDescriptorSetLayout global_set_layout)
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline();
    }

    PointLightSystem::PointLightSystem(std::vector<VkDescriptorSetLayout> global_set_layout)
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline();
    }

    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(VKDevice::Get().device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(des_set_layout.size());
        pipelineLayoutInfo.pSetLayouts = des_set_layout.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(VKDevice::Get().device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
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

        if (vkCreatePipelineLayout(VKDevice::Get().device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }
    void PointLightSystem::CreatePipeline()
    {

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.attribute_descriptions.clear();
        pipelineConfig.bind_descriptions.clear();
        pipelineConfig.renderPass = VKRenderer::Get().GetSwapchainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VKPipeline>(pipelineConfig, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/point_light.vert.spv", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/point_light.frag.spv");
    }

    void PointLightSystem::Update(FrameInfo &frame_info,  SceneRenderInfo &scene_info, GlobalUBO &ubo)
    {
        auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            frame_info.frame_time,
            {0.f, -1.f, 0.f});

        int light_index = 0;
        for (auto &kv : scene_info.entities)
        {
            auto &_obj = kv.second;
            if (!_obj.HasComponent<PointLightComponent>())
                continue;

            assert(light_index < MAX_LIGHTS && "Point light exceeds maximum number of lights!");

            auto _transform = _obj.GetComponent<TransformComponent>();

            _transform.translation = glm::vec3(rotateLight * glm::vec4(_transform.translation, 1.0f));
            ubo.point_lights[light_index].position = glm::vec4(_transform.translation, 1.0f);
            ubo.point_lights[light_index].color = glm::vec4(_obj.GetComponent<PointLightComponent>().color, _obj.GetComponent<PointLightComponent>().light_intensity);
            light_index += 1;
        }
        ubo.num_lights = light_index;
    }

    void PointLightSystem::render(FrameInfo &frame_info, SceneRenderInfo &scene_info)
    {
        auto camera = scene_info.m_Camera.GetComponent<CameraComponent>().m_Camera;

        std::map<float, uint64_t> sorted;
        for (auto &kv : scene_info.entities)
        {
            auto &_object = kv.second;
            if (!_object.HasComponent<PointLightComponent>())
                continue;

            auto _transform = _object.GetComponent<TransformComponent>();
            auto offset = camera.GetPosition() - _transform.translation;
            float disSquared = glm::dot(offset, offset);
            sorted[disSquared] = _object.GetComponent<IDComponent>().id;
        }

        pipeline->Bind(frame_info.command_buffer);
        for (auto &des : scene_info.descriptor_sets)
        {
            if (des.second == VK_NULL_HANDLE)
            {
                VV_CORE_ASSERT(true, "descriptor set is null");
            }
            vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout, des.first, 1, &des.second, 0, nullptr);
        }

        // render with revserse of sorted order
        for (auto it = sorted.rbegin(); it != sorted.rend(); it++)
        {
            auto &_obj = scene_info.entities.at(it->second);
            auto _transform = _obj.GetComponent<TransformComponent>();

            PointLightPushConstants push{};
            push.position = glm::vec4(_transform.translation, 1.0f);
            push.color = glm::vec4(_obj.GetComponent<PointLightComponent>().color, _obj.GetComponent<PointLightComponent>().light_intensity);
            push.radius = _obj.GetComponent<PointLightComponent>().radius;

            vkCmdPushConstants(frame_info.command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);

            vkCmdDraw(frame_info.command_buffer, 6, 1, 0, 0);
        }
    }
} // namespace VectorVertex
