#include "vk_mesh_render_system.hpp"
#include <Components.hpp>
#include <vk_api_data.hpp>
#include <GraphicsContext.hpp>
#include <Log.h>
#include <Entity.hpp>
#include <FrameBuffer.hpp>

namespace VectorVertex
{
    VulkanMeshRenderer::VulkanMeshRenderer(std::vector<VkDescriptorSetLayout> layouts)
    {
        CreatePipelineLayout(layouts);
        // CreatePipeline("/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.vert.spv", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.frag.spv");
        CreatePipeline("/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.vert", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.frag");
    }
    VulkanMeshRenderer::VulkanMeshRenderer(std::vector<VkDescriptorSetLayout> layouts, FrameBuffer &framebuffer)
    {
        CreatePipelineLayout(layouts);
        // CreatePipeline(framebuffer, "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.vert.spv", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.frag.spv");
        CreatePipeline(framebuffer, "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.vert", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/default.frag");
    }
    VulkanMeshRenderer::VulkanMeshRenderer(std::vector<VkDescriptorSetLayout> layouts, std::string vertex_shader, std::string fragment_shader)
    {
        CreatePipelineLayout(layouts);
        CreatePipeline(vertex_shader, fragment_shader);
    }
    void VulkanMeshRenderer::Update(std::unordered_map<UUID, Entity> objects, FrameInfo &info)
    {
    }
    void VulkanMeshRenderer::Render(std::unordered_map<UUID, Entity> objects, FrameInfo &info)
    {

        pipeline->Bind(info.command_buffer);

        for (auto &kv : objects)
        {
            auto &obj = kv.second;
            if (!obj.HasComponent<MeshComponent>()) // Preformance drop when go through all objects
                continue;

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

            vkCmdPushConstants(info.command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
            {
                auto des_set = VulkanAPIData::Get().m_global_descriptor_sets[info.frame_index];
                if (des_set == VK_NULL_HANDLE)
                {
                    VV_CORE_ASSERT(true, "descriptor set is null");
                }
                vkCmdBindDescriptorSets(info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipelineLayout, 0, 1, &des_set, 0, nullptr);
            }

            if (!reinterpret_cast<VKSwapChain *>(GraphicsContext::Get()->GetSwapchain())->isWaitingForFence)
            {
                VVTexture *tex = &VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID);
                if (obj.HasComponent<TextureComponent>() && tex != nullptr)
                {
                    TextureData data = VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID).data;
                    if (data.m_descriptorSet == VK_NULL_HANDLE)
                    {
                        VV_CORE_ERROR("Null Descriptors in Texture :{0}, Entity :{1} ", data.m_Name, obj.GetComponent<IDComponent>().m_Name);
                        // VV_CORE_ASSERT(false, "Texture Descriptorset is NULL!");
                    }
                    else
                    {

                        vkCmdBindDescriptorSets(info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                pipelineLayout, 1, 1, &data.m_descriptorSet, 0, nullptr);
                    }
                }
                
                obj.GetComponent<MeshComponent>().m_Model->BindWithEntityID(info.command_buffer, (int)obj.GetEntt());
                obj.GetComponent<MeshComponent>().m_Model->Draw(info.command_buffer);
            }
        }
    }
    void VulkanMeshRenderer::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

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
}