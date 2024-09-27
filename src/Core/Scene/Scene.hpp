#pragma once
#include <entt.hpp>
#include <Components.hpp>
#include <Base.h>
#include <vv_offscreen.hpp>
#include <vv_point_light_system.hpp>
#include <vv_render_system.hpp>
#include <vv_frame_info.hpp>
#include <Keyboard_inputs.hpp>

namespace VectorVertex
{
    class Entity;


    class Scene
    {
    public:
        Scene(std::string name);
        
        ~Scene();

        Entity CreateEntity(std::string name);
        Entity CreateEntityWithUUID(UUID id,std::string name);
        Entity GetMainCamera();
        void DestroyEntity(Entity entity);
        void DestroyEntityImmidiatly(Entity entity);
        void DeletePendingEntities();

        void OnUpdate();
        void RenderScene();

        std::unordered_map<UUID, Entity> &GetEntities() {return m_Entities;}

    private:
        std::string m_Name;
        entt::registry m_Registry;
        std::unordered_map<UUID, Entity> m_Entities;
        std::unordered_map<UUID, Entity> m_Pending_Delete_Entities;
        


        friend class SceneHierarchy;
        friend class SceneSerializer;
        friend class Entity;
    };
}