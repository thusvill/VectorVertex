#pragma once
#include <Core/Layer.h>
#include <Plattform/Vulkan/vk_pipeline.hpp>
#include <Plattform/Vulkan/vk_device.hpp>
#include <Plattform/Vulkan/vk_camera.hpp>
#include <Plattform/Vulkan/vk_frame_info.hpp>
#include <Plattform/Vulkan/vk_renderer.hpp>

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

namespace VectorVertex
{
    struct ImguiConfig
    {
        ImguiConfig() = default;
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        VkRenderPass renderPass;
        VkInstance instance;
        VkQueue graphicsQueue;
        uint32_t imageCount;
    };
    class Imgui_Layer : public Layer
    {
    public:
        Imgui_Layer();
        ~Imgui_Layer();
        void InitializeImgui(ImguiConfig &config, GLFWwindow *window);
        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void Begin();
        void End(VkCommandBuffer vkCommandBuffer);
         void ShowDockSpace(bool *open);

    private:
        ImguiConfig config;
        GLFWwindow *window;
        ImGui_ImplVulkan_InitInfo init_info{};
        VkDescriptorPool imguiPool;
    };
} // namespace VectorVertex
