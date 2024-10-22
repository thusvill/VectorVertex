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
            FrameInfo info;
            info.command_buffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();
            editor_layer->OnImGuiRender(info);
            {
                auto commandBuffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();
                ImGui::Render();
                ImDrawData *draw_data = ImGui::GetDrawData();

                // Check if there are any ImGui draw commands
                if (draw_data->CmdListsCount > 0)
                {
                    // Iterate over each command list
                    for (int32_t i = 0; i < draw_data->CmdListsCount; ++i)
                    {
                        const ImDrawList *cmd_list = draw_data->CmdLists[i];

                        // Iterate over each command in the command list
                        for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; ++j)
                        {
                            const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];

                            // Example code to execute ImGui draw commands
                            // Bind ImGui textures if needed
                            // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &imguiTextureDescriptorSet, 0, nullptr);

                            // Set scissor rectangle
                            VkRect2D scissorRect;
                            scissorRect.offset = {static_cast<int32_t>(pcmd->ClipRect.x), static_cast<int32_t>(pcmd->ClipRect.y)};
                            scissorRect.extent = {static_cast<uint32_t>(pcmd->ClipRect.z - pcmd->ClipRect.x), static_cast<uint32_t>(pcmd->ClipRect.w - pcmd->ClipRect.y)};
                            vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);

                            // Draw indexed
                            vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, pcmd->IdxOffset, pcmd->VtxOffset, 0);
                        }
                    }
                }
            }
            RenderCommand::EndRenderPass();

            RenderCommand::EndFrame();

            RenderCommand::WaitForDeviceIdl();
        }
    }

} // namespace VectorVertex
