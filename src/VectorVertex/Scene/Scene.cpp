#include "Scene.hpp"
#include "Entity.hpp"
#include <Log.h>
#include <vk_frame_info.hpp>
#include <RenderCommand.hpp>
#include <vk_api_data.hpp>

namespace VectorVertex
{

    Scene::Scene(std::string name) : m_Name(name)
    {
        VV_CORE_INFO("Scene {} Created!", name.c_str());
        // m_RendererSystem = CreateRef<VulkanRendererSystem>();
    }
    void Scene::Init()
    {
    }
    Scene::~Scene()
    {
    }
    Entity Scene::CreateEntity(std::string name)
    {
        Entity entity = {m_Registry.create(), this};
        UUID id = entity.AddComponent<IDComponent>(name).id;
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<MaterialComponent>();
        // entity.AddComponent<TextureComponent>();
        VV_CORE_INFO("Entity Created with name:{0} , ID:{1}", name, entity.GetComponent<IDComponent>().id);
        m_Entities[id] = entity;

        return entity;
    }
    Entity Scene::CreateEntityWithUUID(UUID id, std::string name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(name).id = id;
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<MaterialComponent>();
        // entity.AddComponent<TextureComponent>();
        VV_CORE_INFO("Entity Created with name:{0} , ID:{1}", name, entity.GetComponent<IDComponent>().id);
        m_Entities[id] = entity;

        return entity;
    }
    // Entity& Scene::GetMainCamera()
    // {
    //     Entity cam;

    //     for (auto &kv : m_Entities)
    //     {
    //         if (kv.second.HasComponent<CameraComponent>() && kv.second.GetComponent<CameraComponent>().mainCamera)
    //         {
    //             cam = kv.second;
    //             return cam;
    //             break;
    //         }
    //     }
    //     if (!cam)
    //     {
    //         for (auto &kv : m_Entities)
    //         {
    //             if (kv.second.HasComponent<CameraComponent>())
    //             {
    //                 kv.second.GetComponent<CameraComponent>().mainCamera = true;
    //                 cam = kv.second;
    //                 return cam;
    //                 break;
    //             }
    //         }
    //         if (!cam)
    //         {
    //             cam = CreateEntity("MainCamera");
    //             cam.AddComponent<CameraComponent>().mainCamera = true;
    //             return cam;

    //         }
    //     }
    //     if (!cam.HasComponent<TransformComponent>())
    //     {
    //         cam.AddComponent<TransformComponent>();
    //     }
    //     return cam;
    // }
    void Scene::DestroyEntity(Entity entity)
    {
        // m_Entities.erase(entity.GetComponent<IDComponent>().id);
        // m_Registry.destroy(entity.GetEntt());
        m_Pending_Delete_Entities[entity.GetComponent<IDComponent>().id] = entity;
    }
    void Scene::DestroyEntityImmidiatly(Entity entity)
    {
        m_Entities.erase(entity.GetComponent<IDComponent>().id);
        m_Registry.destroy(entity.GetEntt());
    }
    void Scene::DeletePendingEntities()
    {
        for (auto &kv : m_Pending_Delete_Entities)
        {
            m_Entities.erase(kv.second.GetComponent<IDComponent>().id);
            m_Registry.destroy(kv.second.GetEntt());
        }
        m_Pending_Delete_Entities.clear();
    }
    void Scene::OnUpdate(float frameTime)
    {
        for (auto &kv : m_Entities)
        {
            if (kv.second.HasComponent<CameraComponent>() && kv.second.GetComponent<CameraComponent>().mainCamera)
            {
                m_MainCamera = &kv.second;
                break;
            }
            else
            {
                m_MainCamera = nullptr;
            }
        }
        auto &m_Camera = m_MainCamera->GetComponent<CameraComponent>().m_Camera;

        m_Camera.SetViewYXZ(m_MainCamera->GetComponent<TransformComponent>().translation, m_MainCamera->GetComponent<TransformComponent>().rotation);

        auto aspectRatio = static_cast<float>(m_ViewportSize.width) / static_cast<float>(m_ViewportSize.height); // renderer.GetAspectRatio();
        // camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
        if (m_Camera.GetProjectionType() == VKCamera::ProjectionType::Perspective)
        {
            m_Camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 100.f);
        }

        // m_RendererSystem->OnUpdate(frameTime, m_MainCamera);
    }

    void Scene::RenderScene(FrameInfo &frameInfo)
    {
        GlobalUBO ubo{};
        ubo.view = m_MainCamera->GetComponent<CameraComponent>().m_Camera.GetView();
        ubo.projection = m_MainCamera->GetComponent<CameraComponent>().m_Camera.GetProjection();
        ubo.inverse_view_matrix = m_MainCamera->GetComponent<CameraComponent>().m_Camera.GetInverseViewMatrix();

        // Update point lights


        VulkanAPIData::Get().m_ubo_buffers[frameInfo.frame_index]->writeToBuffer(&ubo);
        VulkanAPIData::Get().m_ubo_buffers[frameInfo.frame_index]->flush();


        // for (auto &kv : m_Entities)
        // {
        //     if (kv.second.HasComponent<MeshComponent>())
        //     {

        //         RenderCommand::DrawMesh(kv.second, frameInfo);
        //     }
        // }

        RenderCommand::DrawScene(m_Entities, frameInfo);

        // m_RendererSystem->OnRender(frameInfo, m_Entities, m_MainCamera);
    }
}