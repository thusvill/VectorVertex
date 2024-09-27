#pragma once
#include <VectorVertex.hpp>
#include <Layer.h>
#include "Imgui_Layer.hpp"
#include "../vv_material.hpp"
#include <SceneHierarchy.hpp>
#include <Entity.hpp>
#include <SceneSerializer.hpp>
namespace VectorVertex
{
    class Scene;
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(VVDevice &device, VVWindow &window, VVRenderer &renderer);
        void SetupImgui(VVDevice *vv_device, VVRenderer *vv_renderer, VVWindow *vv_window);
        ~EditorLayer() = default;
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate() override;
        virtual void OnRender(FrameInfo &frameInfo) override;
        virtual void OnImGuiRender(FrameInfo &frameInfo) override;

        ImTextureID sceneImageView;
        VkExtent2D Viewport_Extent{800, 800};

        bool is_viewport_resized = false;
        Ref<Scene> GetActiveScene() {return m_ActiveScene;}

    private:
        Imgui_Layer imgui_layer;
        ImVec2 prev_size;
        Ref<Scene> m_ActiveScene;
        SceneHierarchy m_SceneHierarchyPanel;
        void UpdateTextures();

    private:
        VVDevice &m_Device;
        VVWindow &m_Window;
        VVRenderer &m_Renderer;

        Scope<VVDescriptorPool> m_global_pool;
        std::vector<Scope<VVBuffer>> m_ubo_buffers;
        std::vector<VkDescriptorSet> m_global_descriptor_sets;
        Scope<VVDescriptorSetLayout> m_global_set_layout;
        Ref<VVOffscreen> m_Offscreen;
        Ref<LveRenderSystem> m_RenderSystem;
        Ref<PointLightSystem> m_PointlightSystem;
        Entity m_SceneCamera;
        VVCamera m_Camera;
        KeyboardInputs camControl{}; // temporyauto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::_V2::system_clock::time_point currentTime;
        float frameTime;
        bool loading_scene=false;
    };
}
