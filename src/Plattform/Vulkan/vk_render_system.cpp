#include "vk_render_system.hpp"
#include <vk_api_data.hpp>
#include <Components.hpp>
#include <Entity.hpp>
#include <RenderCommand.hpp>
#include <GraphicsContext.hpp>
namespace VectorVertex
{
    VulkanRenderSystem::VulkanRenderSystem(std::string vertex_shader, std::string fragment_shader)
    {
        VkDescriptorSetLayout layout = VulkanAPIData::Get().m_global_set_layout->getDescriptorSetLayout();
        CreatePipelineLayout(layout);
        CreatePipeline(vertex_shader, fragment_shader);
    }
    VulkanRenderSystem::VulkanRenderSystem(VkDescriptorSetLayout layout, std::string vertex_shader, std::string fragment_shader)
    {
        CreatePipelineLayout(layout);
        CreatePipeline(vertex_shader, fragment_shader);
    }
    VulkanRenderSystem::VulkanRenderSystem(std::vector<VkDescriptorSetLayout> additional_layouts, std::string vertex_shader, std::string fragment_shader)
    {

        VV_CORE_TRACE("{} Pipeline layouts created", additional_layouts.size());

        CreatePipelineLayout(additional_layouts);
        CreatePipeline(vertex_shader, fragment_shader);
    }
    VulkanRenderSystem::VulkanRenderSystem(std::vector<VkDescriptorSetLayout> additional_layouts, uint32_t push_constant_size, std::string vertex_shader, std::string fragment_shader)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = push_constant_size;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(additional_layouts.size());
        pipelineLayoutInfo.pSetLayouts = additional_layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(VKDevice::Get().device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        CreatePipeline(vertex_shader, fragment_shader);
    }

    void VulkanRenderSystem::Bind(Entity object, FrameInfo info)
    {
        pipeline->Bind(info.command_buffer);
        UploadShaderData(object, info);
    }
    void VulkanRenderSystem::BindLight(Entity object, Entity Camera)
    {
        pipeline->Bind(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());
        UploadLightData(object, Camera);
    }
    void VulkanRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout des_set_layout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        // std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &des_set_layout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(VKDevice::Get().device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void VulkanRenderSystem::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        // std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};

        // std::cout << des_set_layout.size() << std::endl;

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
    void VulkanRenderSystem::CreatePipeline(const std::string vertex_shader, const std::string fragment_shader)
    {

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.renderPass = reinterpret_cast<VkRenderPass>(RenderCommand::GetRendererAPI()->GetRenderpass());
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VKPipeline>(pipelineConfig, vertex_shader, fragment_shader);
    }

    void VulkanRenderSystem::UploadShaderData(Entity object, FrameInfo info)
    {
        auto &obj = object;

        TransformComponent _transform = obj.GetComponent<TransformComponent>();

        PushConstantData push{};
        push.modelMatrix = _transform.mat4();
        push.normalMatrix = _transform.normalMatrix();
        if (obj.HasComponent<MaterialComponent>())
        {
            MaterialComponent _material = obj.GetComponent<MaterialComponent>();
            if (_material.m_ID > -1.0f)
            {
                push.materialData = MaterialLibrary::getMaterial(_material.m_ID).m_MaterialData.getPushData();
            }
            else
            {
                VV_CORE_WARN("Material id {} not found on object: {}", _material.m_ID, obj.GetComponent<IDComponent>().m_Name.c_str());
                push.materialData = MaterialPushConstant{glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)};
            }
        }
        auto command_buffer = info.command_buffer;

        vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);

        std::vector<VkDescriptorSet> des_sets = {VulkanAPIData::Get().m_global_descriptor_sets[info.frame_index]};
        for (int i = 0; i < des_sets.size(); i++)
        {
            if (des_sets[i] == VK_NULL_HANDLE)
            {
                VV_CORE_ASSERT(true, "descriptor set is null");
            }
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout, i, 1, &des_sets[i], 0, nullptr);
        }

        if (!reinterpret_cast<VKSwapChain *>(GraphicsContext::Get()->GetSwapchain())->isWaitingForFence)
        {
            VVTexture *tex = &VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID);
            if (obj.HasComponent<TextureComponent>() && tex != nullptr)
            {
                TextureData data = VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID).data;
                if (data.m_descriptorSet == nullptr)
                {
                    VV_CORE_ERROR("Null Descriptors in Texture :{0}, Entity :{1} ", data.m_Name, obj.GetComponent<IDComponent>().m_Name);
                    // VV_CORE_ASSERT(false, "Texture Descriptorset is NULL!");
                }
                else
                {

                    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            pipelineLayout, 1, 1, &data.m_descriptorSet, 0, nullptr);
                }
            }
            obj.GetComponent<MeshComponent>().m_Model->Bind(info.command_buffer);
            obj.GetComponent<MeshComponent>().m_Model->Draw(info.command_buffer);
        }
    }
    void VulkanRenderSystem::UploadLightData(Entity object, Entity Camera)
    {
        auto camera = Camera.GetComponent<CameraComponent>().m_Camera;

        std::map<float, uint64_t> sorted;
        auto &_object = object;
        if (!_object.HasComponent<PointLightComponent>())
            return;

        auto _transform = _object.GetComponent<TransformComponent>();

        auto command_buffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();

        auto des_sets = VulkanAPIData::Get().m_global_descriptor_sets;
        for (int i = 0; i < des_sets.size(); i++)
        {
            if (des_sets[i] == VK_NULL_HANDLE)
            {
                VV_CORE_ASSERT(true, "descriptor set is null");
            }
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout, i, 1, &des_sets[i], 0, nullptr);
        }

        PointLightPushConstantData push{};
        push.position = glm::vec4(_transform.translation, 1.0f);
        push.color = glm::vec4(object.GetComponent<PointLightComponent>().color, object.GetComponent<PointLightComponent>().light_intensity);
        push.radius = object.GetComponent<PointLightComponent>().radius;

        vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstantData), &push);

        vkCmdDraw(command_buffer, 6, 1, 0, 0);
    }

    void VulkanRenderSystem::UpdateLights(std::unordered_map<UUID, Entity> objects, GlobalUBO ubo)
    {
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
    void VulkanRenderSystem::RenderLights(std::unordered_map<UUID, Entity> objects, Entity Camera)
    {
        auto camera = Camera.GetComponent<CameraComponent>().m_Camera;

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
        auto command_buffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();
        pipeline->Bind(command_buffer);
        auto descriptor_sets = VulkanAPIData::Get().m_global_descriptor_sets;
        for (int i = 0; i < descriptor_sets.size(); i++)
        {
            if (descriptor_sets[i] == VK_NULL_HANDLE)
            {
                VV_CORE_ASSERT(true, "descriptor set is null");
            }
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout, i, 1, &descriptor_sets[i], 0, nullptr);
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

            vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstantData), &push);

            vkCmdDraw(command_buffer, 6, 1, 0, 0);
        }
    }
}
