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
    Application *Application::s_Instance = nullptr;

    Application::Application(ProjectInfo &info) : WIDTH(info.width), HEIGHT(info.height), project_name(info.title)
    {

        VV_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        WindowProps props;
        props.Title = info.title;
        props.Width = info.width;
        props.Height = info.height;

        m_GraphicsContext = GraphicsContext::Create(props);

        VV_CORE_WARN("Application is Started!");
        VV_CORE_WARN("Initializing ...");

        //         editor_layer = new EditorLayer(info);

        //         layers.PushLayer(editor_layer);
        //         editor_layer->SetupImgui();
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

        // while (m_Running && !m_Window->shouldClose())
        while (m_Running)
        {
            // layers.UpdateAll();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            glfwPollEvents();

            RenderCommand::BeginFrame();

            RenderCommand::BeginRenderPass();
            Entity mesh;
            mesh.AddComponent<MeshComponent>("Resources/Models/cube.obj");
            RenderCommand::DrawMesh(mesh.GetComponent<MeshComponent>().GetMeshData());
            RenderCommand::EndRenderPass();

            RenderCommand::EndFrame();

            RenderCommand::WaitForDeviceIdl();
        }
    }

} // namespace VectorVertex
