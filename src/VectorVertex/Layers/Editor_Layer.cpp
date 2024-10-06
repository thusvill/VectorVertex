#include "Editor_Layer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <Entity.hpp>
#include <Scene.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <Utils/PlattformUtils.hpp>
#include <Math/Math.hpp>
namespace VectorVertex
{
    EditorLayer::EditorLayer(ProjectInfo _info) : m_Info(_info), Layer("EditorLayer")
    {
        VV_CORE_INFO("[Layer]:EditorLayer Created!");
    }

    void EditorLayer::SetupImgui()
    {
        VkInstance instance = Application::Get().GetDevice().getInstance();
        VV_CORE_ASSERT(instance, "Vulkan Instance should not be null!");

        ImguiConfig imguiConfig;
        imguiConfig.instance = instance; // Assign Vulkan instance handle
        imguiConfig.Device = Application::Get().GetDevice().device();
        imguiConfig.renderer = &Application::Get().GetRenderer();
        imguiConfig.renderPass = Application::Get().GetRenderer().GetSwapchainRenderPass();
        imguiConfig.PhysicalDevice = Application::Get().GetDevice().getPhysicalDevice();
        imguiConfig.graphicsQueue = Application::Get().GetDevice().graphicsQueue();
        imguiConfig.imageCount = static_cast<uint32_t>(Application::Get().GetRenderer().GetSwapchainImageCount());

        imgui_layer.InitializeImgui(imguiConfig, Application::Get().GetWindow().getGLFWwindow());

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Regular.ttf", 15.f);
        io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Bold.ttf", 15.f);

        io.Fonts->Build();

        VkCommandBuffer command_buffer = Application::Get().GetDevice().beginSingleTimeCommands(); // Vulkan-specific setup
        ImGui_ImplVulkan_CreateFontsTexture();
        Application::Get().GetDevice().endSingleTimeCommands(command_buffer);
        // ImGui_ImplVulkan_DestroyFontUploadObjects();
        {
            ImVec4 *colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
            colors[ImGuiCol_Border] = ImVec4(0.27f, 0.27f, 0.27f, 0.50f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.40f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 0.67f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.44f, 0.46f, 0.50f, 0.31f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.80f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
            colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 0.20f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16f, 0.16f, 0.16f, 0.67f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.44f, 0.44f, 0.95f);
            colors[ImGuiCol_Tab] = ImVec4(0.28f, 0.28f, 0.28f, 0.86f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.27f, 0.30f, 0.80f);
            colors[ImGuiCol_TabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.42f, 0.47f, 0.70f);
            colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
            colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        }
        m_Offscreen = CreateRef<VKOffscreen>(Viewport_Extent);
    }

    void EditorLayer::OnAttach()
    {

        VVMaterialLibrary::InitMaterialLib();
        VVTextureLibrary::InitTextureLib();

        if (!m_Info.path.empty())
        {
            OpenScene(m_Info.path);
        }
        else
        {
            NewScene();
        }

        // m_ActiveScene = CreateRef<Scene>("New Scene");
        // // Entity camera = m_ActiveScene->CreateEntity("Camera");
        // // camera.AddComponent<CameraComponent>();
        // // m_ActiveScene->SetMainCamera(&camera);

        // m_ActiveScene->Init();

        // m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        if (m_ActiveScene)
        {
            VVTextureLibrary::UpdateDescriptors();
        }
    }

    void EditorLayer::OnUpdate()
    {
        RunDeferredActions();
        m_ActiveScene->DeletePendingEntities();
        {
            if (m_SceneHierarchyPanel.requestUpdateTextures)
            {
                VVTextureLibrary::UpdateDescriptors();
                m_SceneHierarchyPanel.requestUpdateTextures = false;
            }
        }

        if (!loading_scene)
        {
            {
                if (is_viewport_resized)
                {
                    m_Offscreen->Resize(Viewport_Extent);
                    if (m_ActiveScene)
                        m_ActiveScene->GetVulkanRenderer()->ResizeViewport(Viewport_Extent);

                    is_viewport_resized = false;
                }
            }

            if (m_ActiveScene)
            {
                m_ActiveScene->OnUpdate(frameTime);
            }
        }
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
            ImGuiStyle &style = ImGui::GetStyle();
            float minwdth = style.WindowMinSize.x;
            style.WindowMinSize.x = 330.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            style.WindowMinSize.x = minwdth;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New"))
                    {
                        RUN_AFTER_FRAME(NewScene());
                    }
                    if (ImGui::MenuItem("Open..."))
                    {
                        RUN_AFTER_FRAME(OpenScene(""));
                    }
                    if (ImGui::MenuItem("SaveAS..."))
                    {
                        SaveSceneAs("");
                    }
                    if (ImGui::MenuItem("Save"))
                    {
                        SaveScene();
                    }

                    if (ImGui::MenuItem("Exit"))
                    {
                        VectorVertex::Application::Get().Close();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
        {
            m_SceneHierarchyPanel.OnImGuiRender();
        }
        { // Inside your ImGui rendering loop
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");

            // camControl.isClickedOnViewport = ImGui::IsWindowHovered() && ImGui::IsMouseDown(1);
            m_ActiveScene->GetVulkanRenderer()->OnImguiViewport();

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

            {
                if (glfwGetMouseButton(Application::Get().GetNativeWindow(), GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
                {
                    if (glfwGetKey(Application::Get().GetNativeWindow(), GLFW_KEY_Q) == GLFW_PRESS)
                    {
                        m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
                    }
                    else if (glfwGetKey(Application::Get().GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS)
                    {
                        m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
                    }
                    else if (glfwGetKey(Application::Get().GetNativeWindow(), GLFW_KEY_E) == GLFW_PRESS)
                    {
                        m_GuizmoType = ImGuizmo::OPERATION::SCALE;
                    }
                    else if (glfwGetKey(Application::Get().GetNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    {
                        m_GuizmoType = -1;
                    }
                }
                

                Entity selected_entity = m_SceneHierarchyPanel.getSelectedEntity();
                if (selected_entity && m_GuizmoType != -1)
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();
                    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, Viewport_Extent.width, Viewport_Extent.height);
                    auto camera = m_ActiveScene->GetMainCamera();
                    const auto &cameraC = camera->GetComponent<CameraComponent>();
                    glm::mat4 proj = cameraC.m_Camera.GetProjection();
                    glm::mat4 flipY = glm::scale(glm::mat4(1.0f), glm::vec3(1, -1, 1));
                    proj = flipY * proj;
                    glm::mat4 cam_View = cameraC.m_Camera.GetView();

                    auto &tc = selected_entity.GetComponent<TransformComponent>();
                    // glm::mat4 transform = tc.mat4();

                    glm::quat tc_rotation = glm::quat(glm::vec3(tc.GetRotationRadians())); // Convert Euler angles to quaternion
                    glm::mat4 rotationMatrix = glm::mat4(tc.rotationQuat);
                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.translation) * rotationMatrix * glm::scale(glm::mat4(1.0f), tc.scale);

                    glm::vec3 original_Rotation = tc.rotation;

                    bool snap = glfwGetKey(Application::Get().GetNativeWindow(), GLFW_KEY_LEFT_CONTROL) || glfwGetKey(Application::Get().GetNativeWindow(), GLFW_KEY_RIGHT_CONTROL);
                    float snapValue = 0.5f;
                    if(m_GuizmoType == ImGuizmo::OPERATION::ROTATE){
                        snapValue = 45.0f;
                    }
                    float snapValues[3] = {snapValue,snapValue,snapValue};

                    ImGuizmo::Manipulate(glm::value_ptr(cam_View), glm::value_ptr(proj), (ImGuizmo::OPERATION)m_GuizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);
                    if (ImGuizmo::IsUsing())
                    {
                        glm::vec3 rotation, position, scale;
                        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

                        // Math::DecomposeTransform(transform, position, rotation, scale);
                        if (m_GuizmoType == ImGuizmo::OPERATION::TRANSLATE)
                        {
                            tc.translation = position;
                        }

                        if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
                        {
                            glm::vec3 delta = rotation - original_Rotation;

                            auto wrapAngle = [](float angle)
                            {
                                while (angle > glm::pi<float>())
                                    angle -= 2.0f * glm::pi<float>();
                                while (angle < -glm::pi<float>())
                                    angle += 2.0f * glm::pi<float>();
                                return angle;
                            };

                            tc.SetRotationEuler(rotation);

                           
                        }
                        if (m_GuizmoType == ImGuizmo::OPERATION::SCALE)
                        {
                            tc.scale = scale;
                        }
                    }
                }
            }

            ImGui::End();
            ImGui::PopStyleVar();
        }

        // ImGui::ShowStyleEditor();

        imgui_layer.End(frameInfo.command_buffer);
    }

    void EditorLayer::NewScene()
    {
        loading_scene = true;
        m_SceneHierarchyPanel.ResetSelectedEntity();
        VVTextureLibrary::ClearLibrary();
        m_ActiveScene = CreateRef<Scene>("New Scene");
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_ActiveScene->Init();
        auto new_Cam = m_ActiveScene->CreateEntity("Camera");
        new_Cam.AddComponent<CameraComponent>();
        m_ActiveScene->SetMainCamera(&new_Cam);
        is_viewport_resized = true;
        
        loading_scene = false;
    }

    void EditorLayer::SaveScene()
    {
        if (!m_Info.path.empty())
        {
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_Info.path);
        }
        else
        {
            SaveSceneAs("");
        }
    }

    void EditorLayer::OpenScene(std::string path)
    {
        if (path.empty())
        {
            path = FileDialog::OpenFile("Open Scene", {"Scene | *.vscene"}, "assets/scene/");
        }
        if (!path.empty())
        {
            loading_scene = true;
            m_SceneHierarchyPanel.ResetSelectedEntity();
            VVTextureLibrary::ClearLibrary();
            m_ActiveScene = CreateRef<Scene>("_temp");
            // m_ActiveScene->Init();
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(path);
            m_ActiveScene->Init();
            VVTextureLibrary::UpdateDescriptors();
            //std::this_thread::sleep_for(std::chrono::milliseconds(2));
            loading_scene = false;
            is_viewport_resized = true;
            m_Info.path = path;
        }
    }

    void EditorLayer::SaveSceneAs(std::string path)
    {
        if (path.empty())
        {

            path = FileDialog::SaveFile("Save Scene", {"Scene | *.vscene"});
        }

        if (!path.empty())
        {
            {
                std::string extension = ".vscene";
                if (path.size() >= extension.size() + 1 && path.compare(path.size() - extension.size(), extension.size(), extension) == 0)
                {
                }
                else
                {
                    path += extension;
                }
            }
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(path);
            m_Info.path = path;
        }
    }

    void EditorLayer::OnRender(FrameInfo &frameInfo)
    {

        if (m_ActiveScene->GetMainCamera() != nullptr && !loading_scene)
        {
            m_Offscreen->StartRenderpass(frameInfo.command_buffer);
            m_ActiveScene->RenderScene(frameInfo);
            m_Offscreen->EndRendrepass(frameInfo.command_buffer);
        }
        sceneImageView = m_Offscreen->getFramebufferImage();
    }

    void EditorLayer::OnDetach()
    {
    }

}