#include "vk_light_render_system.hpp"
#include <Core/Components.hpp>
#include <Plattform/Vulkan/vk_api_data.hpp>
#include <Render/GraphicsContext.hpp>
#include <Core/Log.h>
#include <Scene/Entity.hpp>
namespace VectorVertex
{
    VulkanLightRenderer::VulkanLightRenderer(std::vector<VkDescriptorSetLayout> layouts)
    {
        CreatePipelineLayout(layouts);
        //CreatePipeline("/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.vert.spv", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.frag.spv");
        CreatePipeline("/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.vert", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.frag");
    }

VulkanLightRenderer::VulkanLightRenderer(std::vector<VkDescriptorSetLayout> layouts, FrameBuffer &framebuffer)
{
    CreatePipelineLayout(layouts);
    //CreatePipeline(framebuffer,"/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.vert.spv", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.frag.spv");
    CreatePipeline(framebuffer, "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.vert", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.frag");
}

    VulkanLightRenderer::VulkanLightRenderer(std::vector<VkDescriptorSetLayout> layouts, std::string vertex_shader, std::string fragment_shader)
    {
        CreatePipelineLayout(layouts);
        CreatePipeline(vertex_shader, fragment_shader);
    }

    void VulkanLightRenderer::Update(std::unordered_map<UUID, Entity> objects, FrameInfo& info)
    {
        auto& ubo = info.ubo;
        int light_index = 0;
        for (auto &kv : objects)
        {
            auto &_obj = kv.second;
            if (!_obj.HasComponent<PointLightComponent>())
                continue;

            assert(light_index < MAX_LIGHTS && "Point light exceeds maximum number of lights!");

            auto _transform = _obj.GetComponent<TransformComponent>();

            
            ubo.point_lights[light_index].position = glm::vec4(_transform.translation, 1.0f);
            ubo.point_lights[light_index].color = glm::vec4(_obj.GetComponent<PointLightComponent>().color, _obj.GetComponent<PointLightComponent>().light_intensity);
            light_index += 1;
        }
        ubo.num_lights = light_index;
    }

    void VulkanLightRenderer::Render(std::unordered_map<UUID, Entity> objects, FrameInfo& info)
    {
        if(!m_Camera && !m_Camera->HasComponent<CameraComponent>()){
            VV_CORE_ERROR("No Camera Set to render lights!!");
            return;
        }
        auto camera = m_Camera->GetComponent<CameraComponent>().m_Camera;
        

        std::map<float, uint64_t> sorted;
        for (auto &kv : objects)
        {
            auto &_object = kv.second;
            if (!_object.HasComponent<PointLightComponent>())
                continue;

            auto _transform = _object.GetComponent<TransformComponent>();
            auto offset = camera.GetPosition() - _transform.translation;
            float disSquared = glm::dot(offset, offset);
            sorted[disSquared] = _object.GetComponent<IDComponent>().id;
        }

        pipeline->Bind(info.command_buffer);

        {
            auto des_set = VulkanAPIData::Get().m_global_descriptor_sets[info.frame_index];
            if (des_set == VK_NULL_HANDLE)
            {
                VV_CORE_ASSERT(true, "descriptor set is null");
            }
            vkCmdBindDescriptorSets(info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout, 0, 1, &des_set, 0, nullptr);
        }

        // render with revserse of sorted order
        for (auto it = sorted.rbegin(); it != sorted.rend(); it++)
        {
            auto &_obj = objects.at(it->second);
            auto _transform = _obj.GetComponent<TransformComponent>();

            PointLightPushConstantData push{};
            push.position = glm::vec4(_transform.translation, 1.0f);
            push.color = glm::vec4(_obj.GetComponent<PointLightComponent>().color, _obj.GetComponent<PointLightComponent>().light_intensity);
            push.radius = _obj.GetComponent<PointLightComponent>().radius;

            vkCmdPushConstants(info.command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstantData), &push);

            vkCmdDraw(info.command_buffer, 6, 1, 0, 0);
        }
    }

    void VulkanLightRenderer::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstantData);

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

} // namespace VectorVertex
