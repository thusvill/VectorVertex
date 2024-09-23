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
        entity.AddComponent<TextureComponent>();
        VV_CORE_INFO("Entity Created with name:{0} , ID:{1}", name, entity.GetComponent<IDComponent>().id);
        m_Entities[id] = entity;

        return entity;
    }
    void Scene::OnUpdate()
    {
        
    }

    void Scene::RenderScene()
    {
        
    }
}