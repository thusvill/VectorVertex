#include "Scene.hpp"
#include "Entity.hpp"
#include <Log.h>
#include <vv_frame_info.hpp>

namespace VectorVertex
{
    Scene::Scene(std::string name)
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
    void Scene::DestroyEntity(Entity entity)
    {
        // m_Entities.erase(entity.GetComponent<IDComponent>().id);
        // m_Registry.destroy(entity.GetEntt());
        m_Pending_Delete_Entities[entity.GetComponent<IDComponent>().id] = entity;
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