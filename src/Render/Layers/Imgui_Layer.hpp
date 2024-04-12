#pragma once
#include <Layer.h>
#include <vv_pipeline.hpp>
#include <vv_device.hpp>
#include <vv_camera.hpp>
#include <vv_game_object.hpp>
#include <vv_frame_info.hpp>
#include <vv_renderer.hpp>

#include <vulkan/vulkan.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

namespace VectorVertex
{
    struct ImguiConfig
    {
        ImguiConfig() = default;
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        VVRenderer *renderer;
        VkRenderPass renderPass;
        VkInstance instance;
        VkQueue graphicsQueue;
        uint32_t imageCount;
    };
    class Imgui_Layer : public Layer
    {
    public:
        Imgui_Layer();
        ~Imgui_Layer() = default;
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
