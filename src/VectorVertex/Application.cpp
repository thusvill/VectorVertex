#include "Application.hpp"
#include "Log.h"
#include <Editor_Layer.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>
#include <Renderer.hpp>
#include <RenderCommand.hpp>

namespace VectorVertex
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application *Application::s_Instance = nullptr;

    Application::Application(ProjectInfo &info) : WIDTH(info.width), HEIGHT(info.height), project_name(info.title)
    {

        VV_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        WindowProps props;
        props.Title = info.title;
        props.Width = info.width;
        props.Height = info.height;

        m_GraphicsContext = GraphicsContext::Create();

        m_Window = Window::Create(props);
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        VV_CORE_WARN("Application is Started!");
        VV_CORE_WARN("Initializing ...");

        editor_layer = new EditorLayer(info);

        layers.PushLayer(editor_layer);
        editor_layer->SetupImgui();
        VV_CORE_WARN("Initialized!");
    }
    void Application::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        //        VV_CORE_TRACE("{0}", e.ToString());
    }

    bool Application::OnWindowClose(WindowCloseEvent &e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &e)
    {
        RenderCommand::WindowResize();
        VV_CORE_INFO("{0}", e.ToString());
        return true;
    }

    Application::~Application()
    {
    }

    void Application::run()
    {

        auto currentTime = std::chrono::high_resolution_clock::now();

        // while (m_Running && !m_Window->shouldClose())
        while (m_Running)
        {
            // layers.UpdateAll();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            glfwPollEvents();

            editor_layer->OnUpdate();

            if (RenderCommand::BeginFrame())
            {
                FrameInfo info;
                info.command_buffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();
                info.frame_index = RenderCommand::GetRendererAPI()->GetCurrentFrameIndex();
                info.frame_time = frameTime;

                editor_layer->OnRender(info);

                RenderCommand::BeginRenderPass();

                editor_layer->OnImGuiRender(info);

                RenderCommand::EndRenderPass();
            }
            RenderCommand::EndFrame();

            RenderCommand::WaitForDeviceIdl();
        }
    }

} // namespace VectorVertex
