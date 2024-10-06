#include "Application.hpp"
#include "Log.h"
#include <Editor_Layer.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>
#include <Renderer.hpp>

namespace VectorVertex
{
    Application *Application::s_Instance = nullptr;

        Application::Application(ProjectInfo &info) : WIDTH(info.width), HEIGHT(info.height), project_name(info.title)
    {
        VV_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        VV_CORE_WARN("Application is Started!");
        VV_CORE_WARN("Initializing ...");

        editor_layer = new EditorLayer(info);

        layers.PushLayer(editor_layer);
        editor_layer->SetupImgui();
        VV_CORE_WARN("Initialized!");
        WindowResizeEvent e(info.width, info.height);
        VV_TRACE(e.ToString());
    }

    Application::~Application()
    {
    }

    void Application::run()
    {

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (m_Running && !Window::shouldClose())
        {
            layers.UpdateAll();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            VVTextureLibrary::UpdateDescriptors();

            glfwPollEvents();

            Renderer::StartScene();

            editor_layer->OnRender();

            Renderer::BeginFrame();
            editor_layer->OnImGuiRender();
            Renderer::EndFrame();

            Renderer::EndScene();
            
        }

        Renderer::WaitForIdle();
    }

} // namespace VectorVertex
