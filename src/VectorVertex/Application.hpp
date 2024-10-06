#pragma once
#include <vvpch.hpp>
#include <vk_window.hpp>
#include <vk_device.hpp>
#include <vk_renderer.hpp>
#include <vk_buffer.hpp>
#include <vk_game_object.hpp>
#include <vk_camera.hpp>
#include <vk_descriptors.hpp>
#include <vk_material.hpp>
#include <vk_offscreen.hpp>
// #include "../Render/vv_framebuffer.hpp"

#include <vk_texture.hpp>
#include "Keyboard_inputs.hpp"
#include "LayerStack.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <ApplicationEvent.hpp>

namespace VectorVertex
{
    class EditorLayer;

    struct ProjectInfo
    {
        int width = 1980;
        int height = 1080;
        std::string title = "VectorVertex";
        std::string path = "";
    };

    class Application
    {
    public:
        Application(ProjectInfo &info);
        ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        GLFWwindow *GetNativeWindow() { return VKWindow.getGLFWwindow(); }
        VKDevice &GetDevice() { return VKDevice; }
        VKWindow &GetWindow() { return VKWindow; }
        VKRenderer &GetRenderer() {return renderer;}

        void Close()
        {
            m_Running = false;
        }

        int WIDTH, HEIGHT;
        std::string project_name;
        void run();
        static Application &Get() { return *s_Instance; }

    private:
        static Application *s_Instance;
        bool m_Running = true;
        VKWindow VKWindow{WIDTH, HEIGHT, project_name};
        VKDevice VKDevice{VKWindow};
        VKRenderer renderer{VKWindow, VKDevice};

        EditorLayer *editor_layer;

        LayerStack layers{};

        // Scope<VVDescriptorSetLayout> textureImageDescriptorLayout{};
        // VVTexture base_texture;
    };
} // namespace VectorVertex
