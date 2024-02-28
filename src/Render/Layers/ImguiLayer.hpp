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
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        VVRenderer renderer;
    };
    class ImguiLayer : Layer
    {
    public:
        ImguiLayer() = default;
        ImguiLayer(ImguiConfig &config, GLFWwindow *window) : config(config), Layer("ImguiLayer")
        {
            init_info.PhysicalDevice = config.PhysicalDevice;
            init_info.Device = config.Device;
            InitImgui(window);
        }
        ~ImguiLayer();
        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void Begin();
        void End();
        void ShowDockSpace(bool *open);

    private:
        void InitImgui(GLFWwindow *window);
        ImguiConfig &config;
        ImGui_ImplVulkan_InitInfo init_info;
    };
} // namespace VectorVertex
