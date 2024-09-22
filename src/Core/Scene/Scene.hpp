#pragma once
#include <entt.hpp>
#include <Components.hpp>

namespace VectorVertex
{   
    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(std::string name);

        void OnUpdate();

    private:
        entt::registry m_Registry;

        friend class Entity;
    };
}