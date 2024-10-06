#include "VulkanRendererSystem.hpp"
#include <Entity.hpp>
namespace VectorVertex
{
    void VulkanRendererSystem::Create()
    {
        VVTextureLibrary::UpdateDescriptors();
        m_global_pool = VKDescriptorPool::Builder(VKDevice::Get())
                            .setMaxSets(VKSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VKSwapChain::MAX_FRAMES_IN_FLIGHT)
                            //.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VKSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .build();

        m_ubo_buffers.resize(VKSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < m_ubo_buffers.size(); i++)
        {
            m_ubo_buffers[i] = std::make_unique<VKBuffer>(sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            m_ubo_buffers[i]->map();
        }

        m_global_set_layout = VKDescriptorSetLayout::Builder(VKDevice::Get())
                                  .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                  //.addBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                  .build();

        m_global_descriptor_sets.resize(VKSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_global_descriptor_sets.size(); i++)
        {
            auto buffer_info = m_ubo_buffers[i]->descriptorInfo();

            VKDescriptorWriter(*m_global_set_layout, *m_global_pool)
                .writeBuffer(0, &buffer_info)
                // add texture sampler here
                .build(m_global_descriptor_sets[i]);
        }

            std::vector<VkDescriptorSetLayout> layouts = {m_global_set_layout->getDescriptorSetLayout(), VVTextureLibrary::textureImageDescriptorLayout->getDescriptorSetLayout()};

            m_RenderSystem = CreateRef<LveRenderSystem>(layouts);
            m_PointlightSystem = CreateRef<PointLightSystem>(layouts);
        
    }

    void VulkanRendererSystem::OnRender(FrameInfo &frameInfo, std::unordered_map<UUID, Entity> m_Entities, Entity *m_SceneCamera)
    {

        
        if (m_SceneCamera == nullptr)
        {
            return;
        }

        if (!m_SceneCamera->HasComponent<CameraComponent>())
        {
            return;
        }
        
        {
            auto &cc = m_SceneCamera->GetComponent<CameraComponent>();
            std::unordered_map<int, VkDescriptorSet> descriptor_sets;
            descriptor_sets[0] = m_global_descriptor_sets[frameInfo.frame_index];
            

            std::vector<std::reference_wrapper<VKDescriptorPool>> pools = {*m_global_pool};

            SceneRenderInfo sceneInfo{
                m_Entities,
                *m_SceneCamera,
                descriptor_sets,
                pools};

            // update
            GlobalUBO ubo{};
            ubo.view = cc.m_Camera.GetView();
            ubo.projection = cc.m_Camera.GetProjection();
            ubo.inverse_view_matrix = cc.m_Camera.GetInverseViewMatrix();

            m_PointlightSystem->Update(frameInfo, sceneInfo, ubo);

            m_ubo_buffers[frameInfo.frame_index]->writeToBuffer(&ubo);
            m_ubo_buffers[frameInfo.frame_index]->flush();

            // Offscreen Renderer
            {

                m_RenderSystem->renderGameobjects(frameInfo, sceneInfo);
                m_PointlightSystem->render(frameInfo, sceneInfo);
            }
        }
    }

    void VulkanRendererSystem::OnUpdate(float frameTime, Entity *m_SceneCamera)
    {
        auto newTime = std::chrono::high_resolution_clock::now();
        frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        if (m_SceneCamera == nullptr || !m_SceneCamera->HasComponent<CameraComponent>())
        {
            return;
        }
        
        {
            auto &m_Camera = m_SceneCamera->GetComponent<CameraComponent>().m_Camera;

            camControl.moveInPlaneXZ(static_cast<GLFWwindow*>(VKRenderer::Get().GetWindow().GetNativeWindow()), frameTime, m_SceneCamera->GetComponent<TransformComponent>());
            m_Camera.SetViewYXZ(m_SceneCamera->GetComponent<TransformComponent>().translation, m_SceneCamera->GetComponent<TransformComponent>().rotation);

            auto aspectRatio = static_cast<float>(m_ViewportSize.width) / static_cast<float>(m_ViewportSize.height); // renderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
            if (m_Camera.GetProjectionType() == VKCamera::ProjectionType::Perspective)
            {
                m_Camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 100.f);
            }
            else
            {
                m_Camera.SetOrthographicProjection(static_cast<float>(m_ViewportSize.width), static_cast<float>(m_ViewportSize.height), 0.1f, 100.0f);
            }
        }
    }

    void VulkanRendererSystem::OnImguiViewport()
    {
        camControl.isClickedOnViewport = ImGui::IsWindowHovered() && ImGui::IsMouseDown(1);
    }

} // namespace VectorVertex
