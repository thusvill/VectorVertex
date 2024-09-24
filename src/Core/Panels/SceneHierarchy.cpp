#include "SceneHierarchy.hpp"
#include <Entity.hpp>
#include <string.h>
#include <glm/gtc/type_ptr.hpp>

namespace VectorVertex
{

    std::queue<std::function<void()>> deferredActions;

    void RunDeferredActions()
    {
        while (!deferredActions.empty())
        {
            deferredActions.front()();
            deferredActions.pop();
        }
    }

    SceneHierarchy::SceneHierarchy(const Ref<Scene> &scene)
    {
        SetContext(scene);
    }
    void SceneHierarchy::SetContext(const Ref<Scene> &scene)
    {
        m_Context = scene;
    }
    void SceneHierarchy::OnImGuiRender(VVDevice &device)
    {
        ImGui::Begin("Scene Hierarchy");

        for (auto entt : m_Context->m_Registry.view<entt::entity>())
        {
            Entity entity{entt, m_Context.get()};
            DrawEntityNode(entity);
        }
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_SelectedEntity = {};
        }
        if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
            {
                m_Context->CreateEntity("new Entity");
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectedEntity)
        {
            DrawComponents(device, m_SelectedEntity);

            if (ImGui::Button("AddComponent"))
            {
                ImGui::OpenPopup("AddComponent");
            }

            if (ImGui::BeginPopup("AddComponent"))
            {
                if (ImGui::MenuItem("Mesh Renderer"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<MeshComponent>(device, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/cube.obj"));
                    RUN_AFTER_FRAME(requestUpdateTextures = true);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Point Light"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<PointLightComponent>());
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }
    void SceneHierarchy::DrawEntityNode(Entity entity)
    {
        auto &ID = entity.GetComponent<IDComponent>();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
        bool opened = ImGui::TreeNodeEx((void *)(uint64_t)ID.id, flags, ID.m_Name.c_str());
        bool isEntityDeleted = false;
        if (ImGui::IsItemClicked())
        {

            m_SelectedEntity = entity;
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                isEntityDeleted = true;
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGui::TreePop();
        }

        if (isEntityDeleted)
        {
            if (m_SelectedEntity == entity)
            {
                m_SelectedEntity = {};
            }
            RUN_AFTER_FRAME(m_Context->DestroyEntity(entity));
        }
    }
    void SceneHierarchy::DrawComponents(VVDevice &device, Entity entity)
    {
        if (entity.HasComponent<IDComponent>())
        {
            auto &name = entity.GetComponent<IDComponent>().m_Name;
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, name.c_str());
            if (ImGui::InputText("Entity", buffer, sizeof(buffer)))
            {
                name = std::string(buffer);
            }
        }
        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx((void *)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
            {
                auto &transform = entity.GetComponent<TransformComponent>();
                ImGui::DragFloat3("Position", glm::value_ptr(transform.translation), 0.1f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.1f);
                ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);

                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<CameraComponent>())
        {
            if (ImGui::TreeNodeEx((void *)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
            {
                auto &camera = entity.GetComponent<CameraComponent>().m_Camera;
                const char *projectionTypeStrings[] = {"Orthographic", "Perspective"};
                const char *currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.SetProjectionType((VVCamera::ProjectionType)i);
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<PointLightComponent>())
        {
            if (ImGui::TreeNodeEx((void *)typeid(PointLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Point Light"))
            {
                auto &light = entity.GetComponent<PointLightComponent>();
                ImGui::ColorEdit3("Color ##light", glm::value_ptr(light.color));
                ImGui::DragFloat("Intensity", &light.light_intensity, 0.1f);
                ImGui::DragFloat("Radius", &light.radius, 0.1f);

                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<MaterialComponent>())
        {
            if (ImGui::TreeNodeEx((void *)typeid(MaterialComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Material"))
            {
                auto material = VVMaterialLibrary::getMaterial(entity.GetComponent<MaterialComponent>().m_ID);
                if (ImGui::ColorEdit4("Material Color ##material", glm::value_ptr(material.m_MaterialData.color)))
                {
                    VVMaterialLibrary::updateMaterial(material.m_MaterialData.m_ID, material.m_MaterialData);
                }

                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<MeshComponent>())
        {
            if (ImGui::TreeNodeEx((void *)typeid(MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer"))
            {
                auto &mesh = entity.GetComponent<MeshComponent>();
                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, mesh.path.c_str());

                if (ImGui::InputText("##Entity", buffer, sizeof(buffer)))
                {
                    mesh.path = std::string(buffer);
                }
                if (ImGui::Button("Load"))
                {

                    RUN_AFTER_FRAME(mesh.UpdateMesh(device));
                    RUN_AFTER_FRAME(requestUpdateTextures = true);

                }

                ImGui::TreePop();
            }
        }
    }
}