#include "vk_render_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>
#include <Entity.hpp>
#include <Renderer.hpp>
#include <GraphicsContext.hpp>
#include <Application.hpp>
namespace VectorVertex
{

    LveRenderSystem::LveRenderSystem(VkDescriptorSetLayout global_set_layout)
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline();
        // texture_layout = LveDescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }
    LveRenderSystem::LveRenderSystem(std::vector<VkDescriptorSetLayout> global_set_layout)
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline();
        // texture_layout = LveDescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }

    LveRenderSystem::~LveRenderSystem()
    {
        vkDestroyPipelineLayout(VKDevice::Get().device(), pipelineLayout, nullptr);
    }

    void LveRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout des_set_layout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

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

    void LveRenderSystem::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> des_set_layout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        // std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};

        //std::cout << des_set_layout.size() << std::endl;

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
    void LveRenderSystem::CreatePipeline()
    {

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.renderPass = VKRenderer::Get().GetSwapchainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VKPipeline>(pipelineConfig, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/default.vert.spv", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/default.frag.spv");
    }

    void LveRenderSystem::renderGameobjects(FrameInfo &frame_info, SceneRenderInfo &scene_info)
    {
        pipeline->Bind(frame_info.command_buffer);

        for (auto &kv : scene_info.entities)
        {
            auto &obj = kv.second;
            if (!obj.HasComponent<MeshComponent>()) // Preformance drop when go through all objects
                continue;

            TransformComponent _transform = obj.GetComponent<TransformComponent>();

            SimplePushConstantData push{};
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

            vkCmdPushConstants(frame_info.command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            for (auto &des : scene_info.descriptor_sets)
            {
                if (des.second == VK_NULL_HANDLE)
                {
                    VV_CORE_ASSERT(true, "descriptor set is null");
                }
                vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipelineLayout, des.first, 1, &des.second, 0, nullptr);
            }

            if (!reinterpret_cast<VKSwapChain*>(GraphicsContext::Get()->GetSwapchain())->isWaitingForFence)
            {
                VVTexture* tex = &VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID);
                if (obj.HasComponent<TextureComponent>() && tex !=nullptr)
                {
                    TextureData data = VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID).data;
                    if (data.m_descriptorSet == nullptr)
                    {
                        VV_CORE_ERROR("Null Descriptors in Texture :{0}, Entity :{1} ", data.m_Name, obj.GetComponent<IDComponent>().m_Name);
                        //VV_CORE_ASSERT(false, "Texture Descriptorset is NULL!");
                    }
                    else
                    {

                        vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                pipelineLayout, 1, 1, &data.m_descriptorSet, 0, nullptr);
                    }
                }
                obj.GetComponent<MeshComponent>().m_Model->Bind(frame_info.command_buffer);
                obj.GetComponent<MeshComponent>().m_Model->Draw(frame_info.command_buffer);
            }
        }
    }
    void LveRenderSystem::renderImGui(VkCommandBuffer commandBuffer)
    {
        // Bind ImGui descriptor sets if needed
        // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &imguiDescriptorSet, 0, nullptr);

        // Render ImGui draw commands
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        // Check if there are any ImGui draw commands
        if (draw_data->CmdListsCount > 0)
        {
            // Iterate over each command list
            for (int32_t i = 0; i < draw_data->CmdListsCount; ++i)
            {
                const ImDrawList *cmd_list = draw_data->CmdLists[i];

                // Iterate over each command in the command list
                for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; ++j)
                {
                    const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];

                    // Example code to execute ImGui draw commands
                    // Bind ImGui textures if needed
                    // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &imguiTextureDescriptorSet, 0, nullptr);

                    // Set scissor rectangle
                    VkRect2D scissorRect;
                    scissorRect.offset = {static_cast<int32_t>(pcmd->ClipRect.x), static_cast<int32_t>(pcmd->ClipRect.y)};
                    scissorRect.extent = {static_cast<uint32_t>(pcmd->ClipRect.z - pcmd->ClipRect.x), static_cast<uint32_t>(pcmd->ClipRect.w - pcmd->ClipRect.y)};
                    vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);

                    // Draw indexed
                    vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, pcmd->IdxOffset, pcmd->VtxOffset, 0);
                }
            }
        }
    }

} // namespace VectorVertex
