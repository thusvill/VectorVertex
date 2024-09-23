#include "SceneHierarchy.hpp"
#include <Entity.hpp>
namespace VectorVertex
{
    SceneHierarchy::SceneHierarchy(const Ref<Scene> &scene)
    {
        SetContext(scene);
    }
    void SceneHierarchy::SetContext(const Ref<Scene> &scene)
    {
        m_Context = scene;
    }
    void SceneHierarchy::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");
        for (auto entt : m_Context->m_Registry.view<entt::entity>())
        {
            Entity entity {entt, m_Context.get()};
            DrawEntityNode(entity);
        }
        ImGui::End();
    }
    void SceneHierarchy::DrawEntityNode(Entity entity)
    {
        auto& ID = entity.GetComponent<IDComponent>();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectedEntityID==ID.id) ? ImGuiTreeNodeFlags_Selected:0);
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)ID.id, flags, ID.m_Name.c_str());
        if(ImGui::IsItemClicked()){
            m_SelectedEntityID = ID.id;
        }
        if(opened){
            ImGui::TreePop();
        }
    }
}