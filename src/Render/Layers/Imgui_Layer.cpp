#include "Imgui_Layer.hpp"
#include<iostream>
#include<iterator>

namespace VectorVertex
{
    Imgui_Layer::Imgui_Layer():Layer("ImguiLayer")
    {
        VV_CORE_INFO("[Layer]:ImguiLayer Created!");
    }

    void Imgui_Layer::InitializeImgui(ImguiConfig &config, GLFWwindow *window)
    {
        this->config = config;
        this->window = window;
        // 1: create descriptor pool for IMGUI
        // the size of the pool is very oversize, but it's copied from imgui demo itself.
        VkDescriptorPoolSize pool_sizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);
        pool_info.pPoolSizes = pool_sizes;

        
        if(vkCreateDescriptorPool(config.Device, &pool_info, nullptr, &imguiPool)){
            VV_CORE_ERROR("Failed to create imgui descriptor pool!");
            return;
        }

        // 2: initialize imgui library

        // this initializes the core structures of imgui
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForVulkan(window, true);

        // this initializes imgui for Vulkan
        //ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = config.instance;
        init_info.RenderPass = config.renderPass;
        init_info.PhysicalDevice = config.PhysicalDevice;
        init_info.Device = config.Device;
        init_info.Queue = config.graphicsQueue;
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info);

        // execute a gpu command to upload imgui font textures

        // immediate_submit([&](VkCommandBuffer cmd)
        //                  { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

        // clear font textures from cpu data
        ImGui_ImplVulkan_DestroyFontsTexture();

        // // add the destroy the imgui created structures
        // _mainDeletionQueue.push_function([=]()
        //                                  {

		// vkDestroyDescriptorPool(_device, imguiPool, nullptr);
		// ImGui_ImplVulkan_Shutdown(); });
    }

    void Imgui_Layer::OnAttach()
    {
        // init_info.PhysicalDevice = config.PhysicalDevice;
        // init_info.Device = config.Device;
        // init_info.RenderPass = config.renderPass;
        // InitImgui(window);
    }

    void Imgui_Layer::OnDetach()
    {
        // // add the destroy the imgui created structures


        vkDestroyDescriptorPool(config.Device, imguiPool, nullptr);
        ImGui_ImplVulkan_Shutdown(); 
    }

    void Imgui_Layer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Imgui_Layer::End()
    {
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        for (int i = 0; i < draw_data->CmdListsCount; ++i)
        {
            const ImDrawList *cmd_list = draw_data->CmdLists[i];
            for (int j = 0; j < cmd_list->CmdBuffer.Size; ++j)
            {
                const ImDrawCmd *cmd = &cmd_list->CmdBuffer[j];
                if (cmd->UserCallback)
                {
                    cmd->UserCallback(cmd_list, cmd);
                }
                else
                {
                    if (auto commandBuffer = config.renderer->BeginFrame())
                    {
                        vkCmdDrawIndexed(commandBuffer, cmd->ElemCount, 1, cmd->IdxOffset, cmd->VtxOffset, 0);
                    }
                    else
                    {
                        VV_CORE_ERROR("Failed to begin a frame");
                    }
                }
            }
        }

    }

    void Imgui_Layer::InitImgui(GLFWwindow *window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_Init(&init_info);
    }

} // namespace VectorVertex
