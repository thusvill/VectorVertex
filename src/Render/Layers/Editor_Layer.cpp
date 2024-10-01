#include "Editor_Layer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <Entity.hpp>
#include <Scene.hpp>
#include <Utils/PlattformUtils.hpp>

namespace VectorVertex
{
    EditorLayer::EditorLayer() : Layer("EditorLayer")
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

        m_Offscreen = CreateRef<VVOffscreen>(Viewport_Extent);
    }

    void EditorLayer::OnAttach()
    {

        VVMaterialLibrary::InitMaterialLib();
        VVTextureLibrary::InitTextureLib();

        m_ActiveScene = CreateRef<Scene>("New Scene");
        // Entity camera = m_ActiveScene->CreateEntity("Camera");
        // camera.AddComponent<CameraComponent>();
        // m_ActiveScene->SetMainCamera(&camera);

        m_ActiveScene->Init();

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

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
            style.WindowMinSize.x = 370.0f;
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
                        NewScene();
                    }
                    if (ImGui::MenuItem("Open..."))
                    {
                        OpenScene("");
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

            ImGui::End();
            ImGui::PopStyleVar();
        }

        imgui_layer.End(frameInfo.command_buffer);
    }

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>("New Scene");
        m_ActiveScene->Init();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
            path = FileDialog::OpenFile("Open Scene", {"Scene | *.vscene"});
        }
        if(!path.empty()){
            loading_scene = true;
            m_ActiveScene = CreateRef<Scene>("_temp");
            m_ActiveScene->Init();
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(path);
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            loading_scene = false;
        }
    }

    void EditorLayer::SaveSceneAs(std::string path)
    {
        if (path.empty())
        {

            path = FileDialog::SaveFile("Save Scene", {"Scene | *.vscene"});
        }

        if(!path.empty()){
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