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

        void OnUpdate();
        void RenderScene();

        std::unordered_map<UUID, Entity> &GetEntities() {return m_Entities;}

    private:
        entt::registry m_Registry;
        std::unordered_map<UUID, Entity> m_Entities;


        friend class SceneHierarchy;
        friend class Entity;
    };
}