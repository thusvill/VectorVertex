#pragma once
#include <Application.hpp>
#include <vk_render_system.hpp>
#include <vk_point_light_system.hpp>
namespace VectorVertex
{
    class VulkanRendererSystem
    {
    public:
        VulkanRendererSystem() = default;
        void Create();
        ~VulkanRendererSystem() {
            if(!m_global_descriptor_sets.empty()){
            m_global_pool->freeDescriptors(m_global_descriptor_sets);
            }

        }

        void OnUpdate(float frameTime, Entity *m_SceneCamera);
        void OnImguiViewport();

        void OnRender(FrameInfo &frameInfo, std::unordered_map<UUID, Entity> m_Entities, Entity *m_SceneCamera);
        void ResizeViewport(VkExtent2D new_size)
        {
            if (new_size.height == 0 || new_size.width == 0)
            {
                m_ViewportSize = {1, 1};
            }
            m_ViewportSize = new_size;
        }

    private:
        VkExtent2D m_ViewportSize = {800, 800};

    private:
        Scope<VKDescriptorPool> m_global_pool;
        std::vector<Scope<VKBuffer>> m_ubo_buffers;
        std::vector<VkDescriptorSet> m_global_descriptor_sets;
        Scope<VKDescriptorSetLayout> m_global_set_layout;

        Ref<LveRenderSystem> m_RenderSystem;
        Ref<PointLightSystem> m_PointlightSystem;
        KeyboardInputs camControl{};
        std::chrono::_V2::system_clock::time_point currentTime;
    };

} // namespace VectorVertex
