#pragma once
#include <vvpch.hpp>
#include "LayerStack.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <Core/Events/Event.hpp>
#include <Core/Events/ApplicationEvent.hpp>
#include <Render/GraphicsContext.hpp>

int main(int argc, char **argv);

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

    class VV_API Application
    {
    public:
        Application(ProjectInfo &info);
        virtual ~Application();

        void Close()
        {
            m_Running = false;
        }

        int WIDTH, HEIGHT;
        std::string project_name;
        void Run();
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

        EditorLayer *editor_layer;

        LayerStack m_LayerStack{};

        Scope<Window> m_Window;

        friend int ::main(int argc, char **argv);
    };

    Application *CreateApplication();
} // namespace VectorVertex
