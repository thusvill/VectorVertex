#pragma once
#include "Scene.hpp"

namespace VectorVertex
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity m_handle, Scene *scene);
        Entity(const Entity &other) = default;

        template <typename T>
        bool HasComponent()
        {
            // if ( m_Scene->m_Registry.valid(m_EntityHandler) &&  m_Scene->m_Registry.any_of<T>(m_EntityHandler))
            // {
            //     return true;
            // }
            // return false;
            return m_Scene->m_Registry.any_of<T>(m_EntityHandler);
        }

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            VV_CORE_ASSERT(!HasComponent<T>(), "Entity already have this component!");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandler, std::forward<Args>(args)...);
        }

        template <typename T>
        T &GetComponent()
        {
            VV_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have this component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandler);
        }
        template <typename T>
        T &GetORCreateComponent()
        {
            if (!HasComponent<T>())
            {
                AddComponent<T>();
            }
            return m_Scene->m_Registry.get<T>(m_EntityHandler);
        }

        template <typename T>
        void RemoveComponent()
        {
            VV_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have this component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandler);
        }

        entt::entity GetEntt() { return m_EntityHandler; }

        operator bool() const { return m_EntityHandler != entt::null; }

        

    private:
        entt::entity m_EntityHandler{entt::null};
        Scene *m_Scene = nullptr;
    };

} // namespace VectorVertex
