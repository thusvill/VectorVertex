#include "Imgui_Layer.hpp"
#include <iostream>
#include <ImGuizmo/ImGuizmo.h>
#include <Render/RenderCommand.hpp>
#include <Plattform/Vulkan/vk_device.hpp>

namespace VectorVertex
{
    Imgui_Layer::Imgui_Layer() : Layer("ImguiLayer")
    {
        VV_CORE_INFO("[Layer]: ImguiLayer Created!");
    }

    Imgui_Layer::~Imgui_Layer()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (config.Device != VK_NULL_HANDLE && imguiPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(config.Device, imguiPool, nullptr);
        }
    }

    void Imgui_Layer::InitializeImgui(ImguiConfig &config, GLFWwindow *window)
    {
        this->config = config;
        this->window = window;

        // 1. Create Descriptor Pool
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
        pool_info.poolSizeCount = (uint32_t)(sizeof(pool_sizes) / sizeof(pool_sizes[0]));
        pool_info.pPoolSizes = pool_sizes;

        if (vkCreateDescriptorPool(config.Device, &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to create imgui descriptor pool!");
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = config.instance;
        init_info.PhysicalDevice = config.PhysicalDevice;
        init_info.Device = config.Device;
        init_info.QueueFamily = VKDevice::Get().findPhysicalQueueFamilies().graphicsFamily;
        init_info.Queue = config.graphicsQueue;
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = config.imageCount;
        // init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        VkFormat colorFormat = static_cast<VKSwapChain *>(RenderCommand::GetRendererAPI()->GetSwapchain())->getSwapChainImageFormat();

        init_info.UseDynamicRendering = true;

        VkPipelineRenderingCreateInfoKHR rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &colorFormat;
    rendering_info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

        // Main Viewport Setup
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo = rendering_info;
        // init_info.PipelineInfoMain.RenderPass = VK_NULL_HANDLE; // Not needed for Dynamic Rendering

        // Secondary Viewports Setup (Required for Docking/Multi-viewport)
        init_info.PipelineInfoForViewports.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.PipelineInfoForViewports.PipelineRenderingCreateInfo = rendering_info;


                ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_3, [](const char* function_name, void* user_data) {
        return vkGetInstanceProcAddr(static_cast<VkInstance>(user_data), function_name);
    }, config.instance);

        if (!ImGui_ImplVulkan_Init(&init_info))
        {
            VV_ERROR("Failed to initialize ImGui for Vulkan!");
            return;
        }else{
            VV_INFO("Initialized ImGui for Vulkan successfully.");
            
        }
    }

    void Imgui_Layer::OnAttach() {}
    void Imgui_Layer::OnDetach() {}

    void Imgui_Layer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void Imgui_Layer::End(VkCommandBuffer vkCommandBuffer)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::Render();

        ImDrawData *draw_data = ImGui::GetDrawData();
        VV_CORE_ASSERT(draw_data, "No draw data!");

        // Draw ImGui to the command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, vkCommandBuffer);

        // Update and Render additional Platform Windows (Viewports)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

} // namespace VectorVertex