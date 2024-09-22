#include "vv_render_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>
namespace VectorVertex
{

    LveRenderSystem::LveRenderSystem(VVDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout global_set_layout) : vvDevice{device}
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline(renderPass);
        // texture_layout = LveDescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }
    LveRenderSystem::LveRenderSystem(VVDevice &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> global_set_layout) : vvDevice{device}
    {
        CreatePipelineLayout(global_set_layout);
        CreatePipeline(renderPass);
        // texture_layout = LveDescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }

    LveRenderSystem::~LveRenderSystem()
    {
        vkDestroyPipelineLayout(vvDevice.device(), pipelineLayout, nullptr);
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

        if (vkCreatePipelineLayout(vvDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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

        std::cout << des_set_layout.size() << std::endl;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(des_set_layout.size());
        pipelineLayoutInfo.pSetLayouts = des_set_layout.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(vvDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }
    void LveRenderSystem::CreatePipeline(VkRenderPass renderPass)
    {

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VVPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VVPipeline>(vvDevice, pipelineConfig, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/default.vert.spv", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Shaders/default.frag.spv");
    }

    void LveRenderSystem::renderGameobjects(FrameInfo &frame_info)
    {
        pipeline->Bind(frame_info.command_buffer);

        for (auto &kv : frame_info.game_objects)
        {
            auto &obj = kv.second;
            if (obj.model == nullptr) // Preformance drop when go through all objects
                continue;
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();
            if (obj.material_id > -1.0f)
            {
                push.materialData = VVMaterialLibrary::getMaterial(obj.material_id).m_MaterialData.getPushData();
            }
            else
            {
                VV_CORE_WARN("Material id {} not found on object: {}", obj.material_id, obj.getId());
                push.materialData = MaterialPushConstant{glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)};
            }

            vkCmdPushConstants(frame_info.command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            for (auto &des : frame_info.descriptor_sets)
            {
                if (des.second == VK_NULL_HANDLE)
                {
                    VV_CORE_ASSERT(true, "descriptor set is null");
                }
                vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipelineLayout, des.first, 1, &des.second, 0, nullptr);
            }
            // if (!frame_info.renderer.Get_Swapchain().isWaitingForFence)
            // {
            //     VV_CORE_TRACE("Fence Done!");
            //     // update textures

            //     auto imageInfo = VVTextureLibrary::GetTexture(obj.texture.data.m_ID).getDescriptorImageInfo();
            //     // auto imageInfo = VVTextureLibrary::GetTexture(newTexture).getDescriptorImageInfo();
            //     VVDescriptorWriter(*VVTextureLibrary::textureImageDescriptorLayout, *VVTextureLibrary::texture_pool)
            //         .writeImage(0, &imageInfo)
            //         .build(obj.texture.data.m_descriptorSet);
            // }
            // else
            // {
            //     VV_CORE_TRACE("Waiting For Fence");
            //     // VVTextureLibrary::texture_pool->resetPool();
            //     // auto imageInfo = VVTextureLibrary::GetTexture(obj.texture.data.m_ID).getDescriptorImageInfo();
            //     // VVDescriptorWriter(*VVTextureLibrary::textureImageDescriptorLayout, *VVTextureLibrary::texture_pool)
            //     //     .writeImage(0, &imageInfo)
            //     //     .build(obj.texture.data.m_descriptorSet);
            // }

            if (!frame_info.renderer.Get_Swapchain().isWaitingForFence)
            {
                vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipelineLayout, 1, 1, &obj.texture.data.m_descriptorSet, 0, nullptr);
                obj.model->Bind(frame_info.command_buffer);
                obj.model->Draw(frame_info.command_buffer);
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
