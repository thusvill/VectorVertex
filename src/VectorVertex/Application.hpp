#pragma once
#include <vvpch.hpp>

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
        
        EditorLayer *editor_layer;

        LayerStack layers{};

        // Scope<VVDescriptorSetLayout> textureImageDescriptorLayout{};
        // VVTexture base_texture;
    };
} // namespace VectorVertex
