#pragma once
#include <Core/vvpch.hpp>
#include "Keyboard_inputs.hpp"
#include "LayerStack.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <Core/Event.hpp>
#include <Events/ApplicationEvent.hpp>
#include <Render/GraphicsContext.hpp>

namespace VectorVertex
{
    // class EditorLayer;

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
        void OnEvent(Event &e);

        static Application &Get() { return *s_Instance; }

        GLFWwindow *GetNativeWindow()
        {
            return m_Window->GetNativeWindow();
        }
        Window *GetWindow()
        {
            return m_Window.get();
        }

        float GetFrameTime() { return frameTime; }

    private:
        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);
        float frameTime;

        Ref<GraphicsContext> m_GraphicsContext;

        static Application *s_Instance;
        bool m_Running = true;

        // EditorLayer *editor_layer;

        LayerStack m_LayerStack{};

        Scope<Window> m_Window;
    };
} // namespace VectorVertex
