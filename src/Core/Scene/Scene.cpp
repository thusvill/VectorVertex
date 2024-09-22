#include "Scene.hpp"
#include "Entity.hpp"
#include <Log.h>

namespace VectorVertex
{
    Scene::Scene()
    {
        VV_CORE_INFO("Scene Created!");
    }
    Scene::~Scene()
    {
    }
    Entity Scene::CreateEntity(std::string name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(name);
        entity.AddComponent<TransformComponent>();
        VV_CORE_INFO("Entity Created with name:{0} , ID:{1}",name,entity.GetComponent<IDComponent>().id);

        return entity;
    }
    void Scene::OnUpdate()
    {
    }
}