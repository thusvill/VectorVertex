#pragma once
#include <Application.hpp>
#include <Layer.h>
#include "Imgui_Layer.hpp"
#include <Material.hpp>
#include <SceneHierarchy.hpp>
#include <Entity.hpp>
#include <SceneSerializer.hpp>
#include <FrameBuffer.hpp>
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
        virtual void OnImGuiRender(FrameInfo &frameInfo) override;

        ImTextureID sceneImageView;
        Extent2D Viewport_Extent{800, 800};

        bool is_viewport_resized = false;
        Ref<Scene> GetActiveScene() { return m_ActiveScene; }

    
    public:
    void NewScene();
    void SaveScene();
    void OpenScene(std::string path);
    void SaveSceneAs(std::string path);

    
    private:
        Imgui_Layer imgui_layer;
        ImVec2 prev_size;
        Ref<Scene> m_ActiveScene;
        SceneHierarchy m_SceneHierarchyPanel;
        Ref<FrameBuffer> m_OffScreen;
        

    private:
        float frameTime;
        bool loading_scene = false;
        ProjectInfo m_Info;
        int m_GuizmoType=-1;
    };
}
