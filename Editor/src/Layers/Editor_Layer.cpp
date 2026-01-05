#include "Editor_Layer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <Core/Utils/PlattformUtils.hpp>
#include <Core/Math/Math.hpp>
#include <Render/RenderCommand.hpp>


namespace VectorVertex
{
    EditorLayer::EditorLayer(ProjectInfo _info) : m_Info(_info), Layer("EditorLayer")
    {
        VV_CORE_INFO("[Layer]:EditorLayer Created!");
    }

    void EditorLayer::SetupImgui()
    {
        VkInstance instance = VKDevice::Get().getInstance();
        VV_CORE_ASSERT(instance, "Vulkan Instance should not be null!");

        ImguiConfig imguiConfig;
        imguiConfig.instance = instance; // Assign Vulkan instance handle
        imguiConfig.Device = VKDevice::Get().device();
        imguiConfig.renderPass = reinterpret_cast<VkRenderPass>(RenderCommand::GetRendererAPI()->GetRenderpass());
        imguiConfig.PhysicalDevice = VKDevice::Get().getPhysicalDevice();
        imguiConfig.graphicsQueue = VKDevice::Get().graphicsQueue();
        imguiConfig.imageCount = RenderCommand::GetRendererAPI()->GetSwapchainImageCount();

        imgui_layer.InitializeImgui(imguiConfig, Application::Get().GetNativeWindow());

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        float fontSize = 14.0f; // *2.0f;
        io.Fonts->AddFontFromFileTTF("/home/bios/VectorVertex/Resources/Fonts/Roboto/Roboto-Bold.ttf", fontSize);
        io.FontDefault = io.Fonts->AddFontFromFileTTF("/home/bios/VectorVertex/Resources/Fonts/Roboto/Roboto-Regular.ttf", fontSize);

        io.Fonts->Build();

        VkCommandBuffer command_buffer = VKDevice::Get().beginSingleTimeCommands(); // Vulkan-specific setup
        // ImGui_ImplVulkan_CreateFontsTexture();  This happens automatically in new IMGUI
        VKDevice::Get().endSingleTimeCommands(command_buffer);
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
        FrameBufferSpecification specs;
        specs.size = Viewport_Extent;
        specs.render_image_index = 0;
        specs.attachments = {FrameBufferFormat::RGBA8, FrameBufferFormat::R32S, FrameBufferFormat::Depth32};
        specs.seperate_renderpass = true;
        specs.hasDepth = true;
        m_OffScreen = FrameBuffer::Create(specs);

        RenderCommand::DedicateToFrameBuffer(m_OffScreen.get());
    }

    void EditorLayer::OnAttach()
    {

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
        m_CameraMoving = cam_control.isMoving();

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
                    m_OffScreen->Resize(Viewport_Extent);
                    if (m_ActiveScene)
                        // m_ActiveScene->GetVulkanRenderer()->ResizeViewport(Viewport_Extent);

                        is_viewport_resized = false;
                }
            }

            if (m_ActiveScene)
            {

                m_ActiveScene->OnUpdate();
            }
        }
    }
    void EditorLayer::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(VV_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(VV_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }
    bool EditorLayer::OnKeyPressed(KeyPressedEvent &e)
    {
        if (e.IsRepeat())
            return false;
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode())
        {
        case Key::N:
            if (control)
            {
                NewScene();
            }
            break;
        case Key::O:
            if (control)
            {
                OpenScene();
            }
            break;
        case Key::S:
        {
            if (control)
            {
                if (shift)
                    SaveSceneAs();
                else
                    SaveScene();
            }
            break;
        }

        case Key::D:
        {
            if (control)
                RUN_AFTER_FRAME(OnDuplicateEntity());

            break;
        }

        case Key::Q:
        {
            if (!ImGuizmo::IsUsing() && !cam_control.isClickedOnViewport)
                m_GuizmoType = -1;
            break;
        }
        case Key::W:
        {
            if (!ImGuizmo::IsUsing() && !cam_control.isClickedOnViewport)
                m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case Key::E:
        {
            if (!ImGuizmo::IsUsing() && !cam_control.isClickedOnViewport)
                m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        case Key::R:
        {
            if (!ImGuizmo::IsUsing() && !cam_control.isClickedOnViewport)
                m_GuizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        }
        case Key::Delete:
        {

            if (m_SceneHierarchyPanel.getSelectedEntity() && m_SceneHierarchyPanel.m_Focused && m_SceneHierarchyPanel.m_ItemFocused)
            {
                
                m_ActiveScene->DestroyEntity(m_SceneHierarchyPanel.getSelectedEntity());
                m_SceneHierarchyPanel.setSelectedEntity({});
            }

            break;
        }
        }
        return false;
    }
    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft && !ImGuizmo::IsOver())
        {
            if (m_ViewportHovered && m_HoveredEntity)
            {
                m_SceneHierarchyPanel.setSelectedEntity(m_HoveredEntity);
            }
        }
        return false;
    }

    void EditorLayer::OnDuplicateEntity()
    {
        

        Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
        if (selectedEntity)
        {
            Entity newEntity = m_ActiveScene->Duplicate(selectedEntity);
            m_SceneHierarchyPanel.setSelectedEntity(newEntity);
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

            auto viewportOffset = ImGui::GetCursorPos();

            ImVec2 windowSize = ImGui::GetContentRegionAvail();

            if (prev_size.x != windowSize.x || prev_size.y != windowSize.y)
            {
                prev_size = windowSize;
                Viewport_Extent = {static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)};
                is_viewport_resized = true;
            }
            Viewport_Extent = {static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)};
            if (m_ActiveScene != nullptr)
            {
                m_ActiveScene->m_ViewportSize = Viewport_Extent;
            }

            ImGui::Image(sceneImageView, windowSize);
            m_ViewportHovered = ImGui::IsWindowHovered();
            m_ViewportFocused = ImGui::IsWindowFocused();

            auto bwindowSize = windowSize;

            ImVec2 minBound = ImGui::GetWindowPos();
            minBound.x += viewportOffset.x;
            minBound.y += viewportOffset.y;

            ImVec2 maxBound = {minBound.x + bwindowSize.x, minBound.y + bwindowSize.y};
            m_ViewportBounds[0] = {minBound.x, minBound.y};
            m_ViewportBounds[1] = {maxBound.x, maxBound.y};

            {
                if (m_ViewportHovered)
                    cam_control.isClickedOnViewport = glfwGetMouseButton(Application::Get().GetNativeWindow(), GLFW_MOUSE_BUTTON_RIGHT);
            }

            {

                

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
                    if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
                    {
                        snapValue = 45.0f;
                    }
                    float snapValues[3] = {snapValue, snapValue, snapValue};

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
        ClearSceneResources();
        m_SceneHierarchyPanel.ResetSelectedEntity();
        VVTextureLibrary::ClearLibrary();
        m_ActiveScene = CreateRef<Scene>("New Scene");
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_ActiveScene->Init();
        auto new_Cam = m_ActiveScene->CreateEntity("Camera");
        new_Cam.AddComponent<CameraComponent>().mainCamera = true;
        m_ActiveScene->SetMainCamera(&new_Cam);
        is_viewport_resized = true;
        m_Info.path = "";

        loading_scene = false;
    }

    void EditorLayer::SaveScene()
    {
        if (!m_Info.path.empty())
        {
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_Info.path);
            VV_INFO("Scene {0} Saved at {1}", m_ActiveScene->GetSceneName(), m_Info.path);
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
            m_HoveredEntity = Entity();

            ClearSceneResources();

            m_ActiveScene = CreateRef<Scene>("_temp");
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);

            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(path);

            
            m_ActiveScene->Init();

            VVTextureLibrary::UpdateDescriptors();

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
    void EditorLayer::ClearSceneResources()
    {
        RenderCommand::ClearResources();
        VVTextureLibrary::ClearLibrary();
        
        VVTextureLibrary::UpdateDescriptors();
    }

    void EditorLayer::OnRender(FrameInfo &frameInfo)
    {

        if (m_ActiveScene->GetMainCamera() != nullptr && !loading_scene)
        {
            cam_control.moveInPlaneXZ(Application::Get().GetNativeWindow(), frameInfo.frame_time, m_ActiveScene->GetMainCamera()->GetComponent<TransformComponent>());

            m_OffScreen->BeginRender();
            m_OffScreen->Bind();
            m_ActiveScene->RenderScene(frameInfo);

            m_OffScreen->Unbind();
            {
                auto [mx, my] = ImGui::GetMousePos();
                mx -= m_ViewportBounds[0].x;
                my -= m_ViewportBounds[0].y;
                glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
                //                my = viewportSize.y - my;

                int mouseX = (int)mx;
                int mouseY = (int)my;

                if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
                {

                    int pixelData = *(int *)m_OffScreen->ReadPixel(1, mouseX, mouseY);
                    m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
                }
            }
            m_OffScreen->EndRender();

            sceneImageView = (ImTextureID)(uint64_t)m_OffScreen->GetFrameBufferImage();
        }
        else
        {
            sceneImageView = (ImTextureID)0;
        }
    }
    void EditorLayer::AfterCommandBuffer()
    {
    }

    void EditorLayer::OnDetach()
    {
    }

}