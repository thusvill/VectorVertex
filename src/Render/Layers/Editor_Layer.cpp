#include "Editor_Layer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <Entity.hpp>

#include <Scene.hpp>

namespace VectorVertex
{
    EditorLayer::EditorLayer() : Layer("EditorLayer")
    {
        VV_CORE_INFO("[Layer]:EditorLayer Created!");
        m_ActiveScene = CreateRef<Scene>();

        
    }

    void EditorLayer::SetupImgui(VVDevice *vv_device, VVRenderer *vv_renderer, VVWindow *vv_window)
    {
        VkInstance instance = vv_device->getInstance();
        VV_CORE_ASSERT(instance, "Vulkan Instance should not be null!");

        ImguiConfig imguiConfig;
        imguiConfig.instance = instance; // Assign Vulkan instance handle
        imguiConfig.Device = vv_device->device();
        imguiConfig.renderer = vv_renderer;
        imguiConfig.renderPass = vv_renderer->GetSwapchainRenderPass();
        imguiConfig.PhysicalDevice = vv_device->getPhysicalDevice();
        imguiConfig.graphicsQueue = vv_device->graphicsQueue();
        imguiConfig.imageCount = static_cast<uint32_t>(vv_renderer->GetSwapchainImageCount());

        imgui_layer.InitializeImgui(imguiConfig, vv_window->getGLFWwindow());

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    void EditorLayer::OnAttach()
    {

    }

    void EditorLayer::OnUpdate()
    {
            m_ActiveScene->OnUpdate();
            auto test_object = m_ActiveScene->CreateEntity("test");
    }

    void EditorLayer::OnImGuiRender(FrameInfo &frameInfo)
    {
        imgui_layer.Begin();

        {
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("VectorVertexEditor", nullptr, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            ImGui::End();
        }

        {

            // MaterialData _data = VVMaterialLibrary::getMaterial("supra_body").m_MaterialData;
            // float col[4];
            // col[0] = _data.color.x;
            // col[1] = _data.color.y;
            // col[2] = _data.color.z;
            // col[3] = _data.color.w;
            // ImGui::ColorPicker4("supra_body:", col);
            // _data.color = glm::vec4(col[0], col[1], col[2], col[3]);
            // VVMaterialLibrary::updateMaterial("supra_body", _data);
            // VV_CORE_INFO("supra_body color: {} {} {} {}", col[0], col[1], col[2], col[3]);
        }

        {
            ImGui::Begin("Transforms");
            for (auto &obj : frameInfo.game_objects)
            {

                ImGui::Text("%s",obj.second.m_Name.c_str());
                ImGui::PushID(obj.second.uuid.id);
                ImGui::DragFloat3("Position: ", glm::value_ptr(obj.second.transform.translation), 0.05f, 0.0f, 100.0f);
                ImGui::PopID();
                ImGui::PushID(obj.second.uuid.id);
                ImGui::DragFloat3("Rotation: ", glm::value_ptr(obj.second.transform.rotation), 0.05f, 0.0f, 100.0f);
                ImGui::PopID();
                ImGui::PushID(obj.second.uuid.id);
                ImGui::DragFloat3("Scale: ", glm::value_ptr(obj.second.transform.scale), 0.05f, 0.0f, 100.0f);
                ImGui::PopID();
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Materials");
            for (auto &obj : frameInfo.game_objects)
            {
                if (VVMaterialLibrary::isMaterialAvailable(obj.second.material_id))
                {
                    ImGui::Text("%s",obj.second.m_Name.c_str());
                    MaterialData _data = VVMaterialLibrary::getMaterial(obj.second.material_id).m_MaterialData;
                    ImGui::PushID(obj.second.material_id);
                    float col[4];
                    col[0] = _data.color.r;
                    col[1] = _data.color.g;
                    col[2] = _data.color.b;
                    col[3] = _data.color.a;
                    if (ImGui::ColorEdit4(_data.m_Name.c_str(), col))
                    {
                        _data.color = glm::vec4(col[0], col[1], col[2], col[3]);
                        VVMaterialLibrary::updateMaterial(obj.second.material_id, _data);
                    }
                    ImGui::PopID();
                }
            }
            ImGui::End();
        }

        // Inside your ImGui rendering loop
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        // Get the size of your ImGui window or set your desired size
        ImVec2 windowSize = ImGui::GetContentRegionAvail();

        if (prev_size.x != windowSize.x || prev_size.y != windowSize.y)
        {
            prev_size = windowSize;
            Viewport_Extent = {static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)};
            is_viewport_resized = true;
        }
        Viewport_Extent = {static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)};
        // Display the offscreen image
        ImGui::Image(sceneImageView, windowSize);

        ImGui::End();
        ImGui::PopStyleVar();

        imgui_layer.End(frameInfo.command_buffer);
    }

    void EditorLayer::OnRender(FrameInfo &frameInfo)
    {
    }

    void EditorLayer::OnDetach()
    {
    }

} // namespace VectorVertex