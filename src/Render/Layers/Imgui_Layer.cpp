#include "Imgui_Layer.hpp"
#include <iostream>
#include <ImGuizmo/ImGuizmo.h>

namespace VectorVertex
{
    Imgui_Layer::Imgui_Layer() : Layer("ImguiLayer")
    {
        VV_CORE_INFO("[Layer]:ImguiLayer Created!");
    }

    Imgui_Layer::~Imgui_Layer()
    {
        // Cleanup ImGui resources
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Destroy Vulkan resources
        if (config.Device != VK_NULL_HANDLE && imguiPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(config.Device, imguiPool, nullptr);
        }
    }

    void Imgui_Layer::InitializeImgui(ImguiConfig &config, GLFWwindow *window)
    {
        // Initialize member variables
        this->config = config;
        this->window = window;

        // Create descriptor pool for ImGui
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

        // Create descriptor pool
        if (vkCreateDescriptorPool(config.Device, &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to create imgui descriptor pool!");
            return;
        }

        // Initialize ImGui library
        ImGui::CreateContext();

        // Initialize ImGui for Vulkan
        ImGui_ImplGlfw_InitForVulkan(window, true);

        // Initialize ImGui Vulkan bindings
        init_info.Instance = config.instance;
        // if(config.instance == VK_NULL_HANDLE)
        // {
        //     VV_CORE_ERROR("Failed to initialize ImGui for Vulkan!");
        //     return;
        // }
        init_info.RenderPass = config.renderPass;
        init_info.PhysicalDevice = config.PhysicalDevice;
        init_info.Device = config.Device;
        init_info.Queue = config.graphicsQueue;
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = config.imageCount;

        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        if (ImGui_ImplVulkan_Init(&init_info) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to initialize ImGui for Vulkan!");
            return;
        }

       
    }

    void Imgui_Layer::OnAttach()
    {
    }

    void Imgui_Layer::OnDetach()
    {
        // // Cleanup ImGui resources
        // ImGui_ImplVulkan_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();

        // // Destroy Vulkan resources
        // if (config.Device != VK_NULL_HANDLE && imguiPool != VK_NULL_HANDLE)
        // {
        //     vkDestroyDescriptorPool(config.Device, imguiPool, nullptr);
        // }
    }

    void Imgui_Layer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        
    }

    void Imgui_Layer::End(VkCommandBuffer vkCommandBuffer)
    {
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        VV_CORE_ASSERT(draw_data, "No draw data!");

        ImGui_ImplVulkan_RenderDrawData(draw_data, vkCommandBuffer);
    }

} // namespace VectorVertex
