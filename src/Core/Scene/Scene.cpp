#include "Scene.hpp"
#include "Entity.hpp"
#include <Log.h>
#include <vv_frame_info.hpp>

namespace VectorVertex
{

    Scene::Scene(std::string name) : m_Name(name)
    {
        VV_CORE_INFO("Scene {} Created!", name.c_str());
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
    Entity Scene::GetMainCamera()
    {
        Entity cam;

        for (auto &kv : m_Entities)
        {
            if (kv.second.HasComponent<CameraComponent>() && kv.second.GetComponent<CameraComponent>().mainCamera)
            {
                cam = kv.second;
            }
        }
        if (!cam)
        {
            for (auto &kv : m_Entities)
            {
                if (kv.second.HasComponent<CameraComponent>())
                {
                    kv.second.GetComponent<CameraComponent>().mainCamera = true;
                    cam = kv.second;
                    break;
                }
            }
            if (!cam)
            {
                cam = CreateEntity("MainCamera");
                cam.AddComponent<CameraComponent>().mainCamera = true;
            }
        }
        if (!cam.HasComponent<TransformComponent>())
        {
            cam.AddComponent<TransformComponent>();
        }
        return cam;
    }
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
    void Scene::OnUpdate()
    {
    }

    void Scene::RenderScene()
    {
    }
}