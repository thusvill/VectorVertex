#include "ImguiLayer.hpp"

namespace VectorVertex
{

    void ImguiLayer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImguiLayer::End()
    {
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        for (int i = 0; i < draw_data->CmdListsCount; ++i)
        {
            const ImDrawList *cmd_list = draw_data->CmdLists[i];
            for (int j = 0; j < cmd_list->CmdBuffer.Size; ++j)
            {
                const ImDrawCmd *cmd = &cmd_list->CmdBuffer[j];
                if (cmd->UserCallback)
                {
                    cmd->UserCallback(cmd_list, cmd);
                }
                else
                {
                    if (auto commandBuffer = config.renderer.BeginFrame())
                    {
                        vkCmdDrawIndexed(commandBuffer, cmd->ElemCount, 1, cmd->IdxOffset, cmd->VtxOffset, 0);
                    }
                    else
                    {
                        VV_CORE_ERROR("Failed to begin a frame");
                    }
                }
            }
        }

        // Cleanup ImGui
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImguiLayer::InitImgui(GLFWwindow *window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_Init(&init_info);
    }

} // namespace VectorVertex
