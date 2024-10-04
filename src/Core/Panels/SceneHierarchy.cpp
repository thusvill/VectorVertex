#include "SceneHierarchy.hpp"
#include <Entity.hpp>
#include <string.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <Utils/PlattformUtils.hpp>

namespace VectorVertex
{

    std::queue<std::function<void()>> deferredActions;

    static void DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[1];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }

    static bool DrawTextfield(const std::string &lable, std::string &value, UUID id = 0)
    {
        ImGui::PushID(id);
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, value.c_str());

        bool changed = ImGui::InputText(lable.c_str(), buffer, sizeof(buffer));
        if (changed)
        {
            value = std::string(buffer);
        }
        ImGui::PopID();
        return changed;
    }

    template <typename T, typename UIFunction>
    static void DrawComponent(const std::string name, Entity entity, UIFunction uiFunction)
    {
        if (entity.HasComponent<T>())
        {
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            float lineHight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void *)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding, name.c_str());
            ImGui::PopStyleVar();

            ImGui::SameLine(contentRegionAvailable.x - lineHight * 0.5f);
            if (ImGui::Button("...", ImVec2(lineHight, lineHight)))
            {
                ImGui::OpenPopup("Component Settings");
            }

            bool remove_component = false;
            if (ImGui::BeginPopup("Component Settings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    remove_component = true;
                }
                ImGui::EndPopup();
            }
            if (open)
            {
                auto &component = entity.GetComponent<T>();
                uiFunction(component);

                ImGui::TreePop();
            }

            if (remove_component)
            {
                entity.RemoveComponent<T>();
            }
        }
    }

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
    void SceneHierarchy::OnImGuiRender()
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
                m_SelectedEntity = m_Context->CreateEntity("new Entity");
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectedEntity)
        {
            DrawComponents(m_SelectedEntity);

            ImGui::Spacing();
            ImGui::Separator();

            if (ImGui::Button("AddComponent"))
            {
                ImGui::OpenPopup("AddComponent");
            }

            if (ImGui::BeginPopup("AddComponent"))
            {
                if (ImGui::MenuItem("Mesh Renderer"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<MeshComponent>("/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/cube.obj"));
                    if(!m_SelectedEntity.HasComponent<TextureComponent>())
                        RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<TextureComponent>());
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Point Light"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<PointLightComponent>());
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Material"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<MaterialComponent>());
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Texture Component"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<TextureComponent>());
                    RUN_AFTER_FRAME(VVTextureLibrary::UpdateDescriptors());
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Camera"))
                {
                    RUN_AFTER_FRAME(m_SelectedEntity.AddComponent<CameraComponent>(false));
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
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
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
            m_Context->DestroyEntity(entity);
        }
    }
    void SceneHierarchy::DrawComponents(Entity entity)
    {

        if (entity.HasComponent<IDComponent>())
        {
            auto &ID = entity.GetComponent<IDComponent>();

            ImGui::Separator();
            // bool open = ImGui::TreeNodeEx((void *)typeid(IDComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap , ID.m_Name.c_str());

            //            if (open)
            //          {

            DrawTextfield("Name", ID.m_Name, ID.id);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), std::to_string(ID.id).c_str());

            //                ImGui::TreePop();
            //        }
        }

        DrawComponent<TransformComponent>("Transform", entity, [](auto &transform)
                                          {
                            DrawVec3Control("Position", transform.translation);
                DrawVec3Control("Rotation", transform.rotation);
                DrawVec3Control("Scale", transform.scale, 1.0f); });

        DrawComponent<CameraComponent>("Camera", entity, [](auto &component)
                                       {
                                           auto &camera = component.m_Camera;
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
                                           ImGui::Checkbox("Is Main Camera", &component.mainCamera); });

        DrawComponent<PointLightComponent>("Point Light", entity, [](auto &light)
                                           {

                ImGui::ColorEdit3("Color ##light", glm::value_ptr(light.color));
                ImGui::DragFloat("Intensity", &light.light_intensity, 0.1f);
                ImGui::DragFloat("Radius", &light.radius, 0.1f); });

        DrawComponent<MaterialComponent>("Material", entity, [](auto &component)
                                         {
                auto material = VVMaterialLibrary::getMaterial(component.m_ID);
                
                if (ImGui::ColorEdit4("Material Color ##material", glm::value_ptr(material.m_MaterialData.color)))
                {
                    VVMaterialLibrary::updateMaterial(material.m_MaterialData.m_ID, material.m_MaterialData);
                } });

        DrawComponent<TextureComponent>("Texture", entity, [](auto &component)
                                        {
                                            auto &texture = VVTextureLibrary::GetTexture(component.m_ID);
                                            DrawTextfield("Name ##TextureName", texture.data.m_Name, texture.data.m_ID);
                                            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
                                            float lineHight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                                            ImGui::SameLine(contentRegionAvailable.x - lineHight * 0.5f);
                                            if(ImGui::Button("New ##NewTexture")){
                                                
                                                UUID new_id = UUID();
                                                RUN_AFTER_FRAME(VVTextureLibrary::CreateWithUUID(new_id,"New Texture", "Resources/Textures/default.png"));                                            
                                                RUN_AFTER_FRAME(VVTextureLibrary::UpdateDescriptors());
                                                VVTextureLibrary::DeleteTexture(component.m_ID);
                                                component.m_ID = new_id;
                                                
                                            }
                                            if (DrawTextfield("##Path", texture.data.m_path, texture.data.m_ID))
                                            {
                                                if (ImGui::Button("Load"))
                                                {
                                                    RUN_AFTER_FRAME(texture.loadTexture(texture.data.m_path));
                                                    RUN_AFTER_FRAME(VVTextureLibrary::UpdateDescriptors());
                                                    
                                                }
                                            }
                                            contentRegionAvailable = ImGui::GetContentRegionAvail();
                                            lineHight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                                            ImGui::SameLine(contentRegionAvailable.x - lineHight * 0.5f);
                                            if (ImGui::Button("... ##Texture", ImVec2(lineHight, lineHight)))
                                            {
                                                std::string new_path = FileDialog::OpenFile("Open Texture", {"Texture | *.png *.jpg"}, "Resources");
                                                if(!new_path.empty()){
                                                    texture.data.m_path = new_path;
                                                    RUN_AFTER_FRAME(texture.loadTexture(texture.data.m_path));
                                                    RUN_AFTER_FRAME(VVTextureLibrary::UpdateDescriptors());
                                                }
                                            } });
        DrawComponent<MeshComponent>("Mesh", entity, [](auto &mesh)
                                     {
                                         if (DrawTextfield("##Path", mesh.path, mesh.m_ID))
                                         {
                                             if (ImGui::Button("Load"))
                                             {

                                                 RUN_AFTER_FRAME(mesh.UpdateMesh());
                                                 RUN_AFTER_FRAME(VVTextureLibrary::UpdateDescriptors());
                                             }
                                         }
                                         ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
                                         float lineHight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                                         ImGui::SameLine(contentRegionAvailable.x - lineHight * 0.5f);
                                         if (ImGui::Button("... ##Mesh", ImVec2(lineHight, lineHight)))
                                         {
                                             std::string new_path = FileDialog::OpenFile("Open Mesh", {"Mesh | *.obj"}, "Resources");
                                             if (!new_path.empty())
                                             {
                                                 mesh.path = new_path;
                                                 RUN_AFTER_FRAME(mesh.UpdateMesh());
                                                 RUN_AFTER_FRAME(VVTextureLibrary::UpdateDescriptors());
                                             }
                                         }
                                     });
    }
}