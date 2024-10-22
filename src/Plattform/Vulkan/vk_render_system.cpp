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
    VulkanRenderSystem::VulkanRenderSystem(std::vector<VkDescriptorSetLayout> additional_layouts, std::string vertex_shader, std::string fragment_shader)
    {
        // std::vector<VkDescriptorSetLayout> layout = {VulkanAPIData::Get().m_global_set_layout->getDescriptorSetLayout()};
        // layout.insert(layout.end(), additional_layouts.begin(), additional_layouts.end());
        std::vector<VkDescriptorSetLayout> layouts;
        if (std::find(additional_layouts.begin(), additional_layouts.end(), VulkanAPIData::Get().m_global_set_layout->getDescriptorSetLayout()) == additional_layouts.end())
        {
            layouts.push_back(VulkanAPIData::Get().m_global_set_layout->getDescriptorSetLayout());
        }
        for (auto &layout : additional_layouts)
        {
            layouts.push_back(layout);
        }

        CreatePipelineLayout(additional_layouts);
        CreatePipeline(vertex_shader, fragment_shader);
    }
    void VulkanRenderSystem::Bind(Entity object)
    {
        pipeline->Bind(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());
        UploadShaderData(object);
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

    void VulkanRenderSystem::UploadShaderData(Entity object)
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
        auto command_buffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();

        vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);

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
        }
    }
}
