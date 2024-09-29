#pragma once

#include "../Render/vv_window.hpp"
#include "../Render/vv_device.hpp"
#include "../Render/vv_renderer.hpp"
#include "../Render/vv_buffer.hpp"
#include "../Render/vv_game_object.hpp"
#include "../Render/vv_camera.hpp"
#include "../Render/vv_descriptors.hpp"
#include "../Render/vv_material.hpp"
#include "../Render/vv_offscreen.hpp"
// #include "../Render/vv_framebuffer.hpp"

#include "../Render/vv_texture.hpp"
#include "Keyboard_inputs.hpp"
#include "LayerStack.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <chrono>
#include <array>

namespace VectorVertex
{
    class EditorLayer;

    struct ProjectInfo
    {
        int width = 1980;
        int height = 1080;
        std::string title = "VectorVertex";
    };

    class Application
    {
    public:
        Application(ProjectInfo &info);
        ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        GLFWwindow *GetNativeWindow() { return vvWindow.getGLFWwindow(); }
        VVDevice &GetDevice() { return vvDevice; }
        VVWindow &GetWindow() { return vvWindow; }
        VVRenderer &GetRenderer() {return renderer;}

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
        VVWindow vvWindow{WIDTH, HEIGHT, project_name};
        VVDevice vvDevice{vvWindow};
        VVRenderer renderer{vvWindow, vvDevice};

        EditorLayer *editor_layer;

        LayerStack layers{};

        // Scope<VVDescriptorSetLayout> textureImageDescriptorLayout{};
        // VVTexture base_texture;
    };
} // namespace VectorVertex
