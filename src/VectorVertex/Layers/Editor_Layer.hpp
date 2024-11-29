#pragma once
#include <Application.hpp>
#include <Layer.h>
#include "Imgui_Layer.hpp"
#include <Material.hpp>
#include <SceneHierarchy.hpp>
#include <Entity.hpp>
#include <SceneSerializer.hpp>
#include <FrameBuffer.hpp>
#include <Event.hpp>
#include <KeyEvent.hpp>
#include <MouseEvent.hpp>
#include <Input.hpp>

namespace VectorVertex
{
    class Scene;
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(ProjectInfo _info);
        void SetupImgui();
        ~EditorLayer() = default;
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate() override;
        
        virtual void OnRender(FrameInfo &frameInfo) override;
        virtual void OnEvent(Event &e) override;
         virtual void OnImGuiRender(FrameInfo &frameInfo) override;
        void AfterCommandBuffer();

        ImTextureID sceneImageView;
        Extent2D Viewport_Extent{800, 800};

        bool is_viewport_resized = false;
        Ref<Scene> GetActiveScene() { return m_ActiveScene; }

    public:
        void NewScene();
        void SaveScene();
        void OpenScene(std::string path = "");
        void SaveSceneAs(std::string path = "");
        void ClearSceneResources();
        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        void OnDuplicateEntity();

    private:
        Imgui_Layer imgui_layer;
        ImVec2 prev_size;
        Ref<Scene> m_ActiveScene;
        SceneHierarchy m_SceneHierarchyPanel;
        Ref<FrameBuffer> m_OffScreen;

        glm::vec2 m_ViewportBounds[2];

        KeyboardInputs cam_control;

    private:
        float frameTime;
        bool loading_scene = false;
        ProjectInfo m_Info;

        Entity m_HoveredEntity;
        int m_GuizmoType = -1;
        bool m_ViewportHovered = false;
        bool m_ViewportFocused = false;
        bool m_CameraMoving = false;
    };
}
