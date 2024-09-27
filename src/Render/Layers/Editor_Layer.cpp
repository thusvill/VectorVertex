#include "Editor_Layer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <Entity.hpp>
#include <Scene.hpp>

namespace VectorVertex
{
    EditorLayer::EditorLayer(VVDevice &device, VVWindow &window, VVRenderer &renderer) : m_Device(device), m_Window(window), m_Renderer(renderer), Layer("EditorLayer")
    {
        VV_CORE_INFO("[Layer]:EditorLayer Created!");

        m_ActiveScene = CreateRef<Scene>("Main");

        m_global_pool = VVDescriptorPool::Builder(m_Device)
                            .setMaxSets(VVSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VVSwapChain::MAX_FRAMES_IN_FLIGHT)
                            //.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VVSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .build();

        m_ubo_buffers.resize(VVSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < m_ubo_buffers.size(); i++)
        {
            m_ubo_buffers[i] = std::make_unique<VVBuffer>(m_Device, sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            m_ubo_buffers[i]->map();
        }

        m_global_set_layout = VVDescriptorSetLayout::Builder(m_Device)
                                  .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                  //.addBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                  .build();

        m_global_descriptor_sets.resize(VVSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_global_descriptor_sets.size(); i++)
        {
            auto buffer_info = m_ubo_buffers[i]->descriptorInfo();

            VVDescriptorWriter(*m_global_set_layout, *m_global_pool)
                .writeBuffer(0, &buffer_info)
                // add texture sampler here
                .build(m_global_descriptor_sets[i]);
        }

        VVMaterialLibrary::InitMaterialLib();
        VVTextureLibrary::InitTextureLib(device);

        std::vector<VkDescriptorSetLayout> layouts = {m_global_set_layout->getDescriptorSetLayout(), VVTextureLibrary::textureImageDescriptorLayout->getDescriptorSetLayout()};

        m_RenderSystem = CreateRef<LveRenderSystem>(m_Device, m_Renderer.GetSwapchainRenderPass(), layouts);
        m_PointlightSystem = CreateRef<PointLightSystem>(m_Device, m_Renderer.GetSwapchainRenderPass(), layouts);
        // m_SceneCamera = m_ActiveScene->CreateEntity("Camera View");
        // m_SceneCamera.AddComponent<CameraComponent>().m_Camera.SetProjectionType(VVCamera::ProjectionType::Perspective);
        // m_SceneCamera.GetComponent<TransformComponent>().translation = glm::vec3(-12.188, -6.700, -9.159);
        // m_SceneCamera.GetComponent<TransformComponent>().rotation = glm::vec3(-0.41, 0.87, 0.0f);

        // {
        //     // load a model
        //     auto light = m_ActiveScene->CreateEntity("Light");
        //     light.AddComponent<PointLightComponent>().color = glm::vec3(1.0f, 1.0f, 1.0f);
        //     light.GetComponent<PointLightComponent>().light_intensity = 255.0f;
        //     light.GetComponent<TransformComponent>().translation = glm::vec3(0.0f, -17.10f, 3.4f);
        //     auto box = m_ActiveScene->CreateEntity("Box");
        //     box.AddComponent<MeshComponent>(m_Device, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/supra/supra.obj");
        //     box.GetComponent<TransformComponent>().rotation = glm::vec3(3.15f, 0.0f, 0.0f);
        //     box.AddComponent<TextureComponent>().m_ID = VVTextureLibrary::Create("new", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Textures/BackgroundGreyGridSprite.png");
        // }

        currentTime = std::chrono::high_resolution_clock::now();
        UpdateTextures();

        // SceneSerializer serializer(m_ActiveScene, device);
        // serializer.Serialize("assets/scene/Example.scene");
        // serializer.Deserialize("assets/scene/Example.scene");

        m_SceneCamera = m_ActiveScene->GetMainCamera();
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

        io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Regular.ttf", 15.f);
        io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Bold.ttf", 15.f);

        io.Fonts->Build();

        VkCommandBuffer command_buffer = vv_device->beginSingleTimeCommands(); // Vulkan-specific setup
        ImGui_ImplVulkan_CreateFontsTexture();
        vv_device->endSingleTimeCommands(command_buffer);
        // ImGui_ImplVulkan_DestroyFontUploadObjects();

        m_Offscreen = CreateRef<VVOffscreen>(m_Device, m_Renderer, Viewport_Extent);
    }

    void EditorLayer::OnAttach()
    {
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnUpdate()
    {
        if (!m_SceneCamera)
        {
            m_SceneCamera = m_ActiveScene->GetMainCamera();
        }

        m_ActiveScene->DeletePendingEntities();
        RunDeferredActions(); // runs commads after frame
        {
            if (m_SceneHierarchyPanel.requestUpdateTextures)
            {
                UpdateTextures();
                m_SceneHierarchyPanel.requestUpdateTextures = false;
            }
        }
        m_ActiveScene->OnUpdate();

        if (!loading_scene)
        {
            auto newTime = std::chrono::high_resolution_clock::now();
            frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            camControl.moveInPlaneXZ(m_Window.getGLFWwindow(), frameTime, m_SceneCamera.GetComponent<TransformComponent>());
            m_Camera.SetViewYXZ(m_SceneCamera.GetComponent<TransformComponent>().translation, m_SceneCamera.GetComponent<TransformComponent>().rotation);

            auto aspectRatio = static_cast<float>(m_Offscreen->getViewSize().width) / static_cast<float>(m_Offscreen->getViewSize().height); // renderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
            if (m_Camera.GetProjectionType() == VVCamera::ProjectionType::Perspective)
            {
                m_Camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 100.f);
            }
            else
            {
                m_Camera.SetOrthographicProjection(static_cast<float>(m_Offscreen->getViewSize().width), static_cast<float>(m_Offscreen->getViewSize().height), 0.1f, 100.0f);
                ;
            }

            {
                if (is_viewport_resized)
                {
                    m_Offscreen->Resize(Viewport_Extent);
                    is_viewport_resized = false;
                }
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
                    if (ImGui::MenuItem("Save"))
                    {
                        SceneSerializer serializer(m_ActiveScene, m_Device);
                        serializer.Serialize("assets/scene/Example.scene");
                    }
                    if (ImGui::MenuItem("Load"))
                    {
                        loading_scene = true;
                        SceneSerializer serializer(m_ActiveScene, m_Device);
                        serializer.Deserialize("assets/scene/Example.scene");
                        m_ActiveScene->DestroyEntityImmidiatly(m_SceneCamera);
                        m_SceneCamera = serializer.m_MainCamera;
                        std::this_thread::sleep_for(std::chrono::milliseconds(2));
                        loading_scene = false;
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
            m_SceneHierarchyPanel.OnImGuiRender(m_Device);
        }
        { // Inside your ImGui rendering loop
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");

            camControl.isClickedOnViewport = ImGui::IsWindowHovered() && ImGui::IsMouseDown(1);

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

    void EditorLayer::UpdateTextures()
    {

        // for (auto &kv : m_ActiveScene->GetEntities())
        // {
        //     auto &obj = kv.second;
        //     if (!m_Renderer.Get_Swapchain().isWaitingForFence)
        //     {
        //         VV_CORE_TRACE("Fence Done!");
        //         // update textures
        //         if (obj.HasComponent<TextureComponent>())
        //         {
        //             auto imageInfo = VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID).getDescriptorImageInfo();
        //             VVDescriptorWriter(*VVTextureLibrary::textureImageDescriptorLayout, *VVTextureLibrary::texture_pool)
        //                 .writeImage(0, &imageInfo)
        //                 .build(VVTextureLibrary::GetTexture(obj.GetComponent<TextureComponent>().m_ID).data.m_descriptorSet);
        //         }
        //     }
        //     else
        //     {
        //         VV_CORE_TRACE("Waiting For Fence");
        //     }
        // }
        VVTextureLibrary::UpdateDescriptors();
    }

    void EditorLayer::OnRender(FrameInfo &frameInfo)
    {
        // UpdateTextures();

        std::unordered_map<int, VkDescriptorSet> descriptor_sets;
        descriptor_sets[0] = m_global_descriptor_sets[frameInfo.frame_index];

        std::vector<std::reference_wrapper<VVDescriptorPool>> pools = {*m_global_pool};

        SceneRenderInfo sceneInfo{
            m_ActiveScene->GetEntities(),
            m_SceneCamera,
            descriptor_sets,
            pools};

        // update
        GlobalUBO ubo{};
        ubo.view = m_Camera.GetView();
        ubo.projection = m_Camera.GetProjection();
        ubo.inverse_view_matrix = m_Camera.GetInverseViewMatrix();

        m_PointlightSystem->Update(frameInfo, sceneInfo, ubo);

        m_ubo_buffers[frameInfo.frame_index]->writeToBuffer(&ubo);
        m_ubo_buffers[frameInfo.frame_index]->flush();

        // Offscreen Renderer
        {

            m_Offscreen->StartRenderpass(frameInfo.command_buffer);
            m_RenderSystem->renderGameobjects(frameInfo, sceneInfo);
            m_PointlightSystem->render(frameInfo, sceneInfo);
            m_Offscreen->EndRendrepass(frameInfo.command_buffer);
            sceneImageView = m_Offscreen->getFramebufferImage();
        }
    }

    void EditorLayer::OnDetach()
    {
    }

} // namespace VectorVertex