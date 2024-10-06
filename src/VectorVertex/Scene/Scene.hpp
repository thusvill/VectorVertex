#pragma once
#include <entt.hpp>
#include <Components.hpp>
#include <Base.h>
#include <vk_offscreen.hpp>
#include <vk_point_light_system.hpp>
#include <vk_render_system.hpp>
#include <vk_frame_info.hpp>
#include <Keyboard_inputs.hpp>
#include <VulkanRendererSystem.hpp>

namespace VectorVertex
{
    class Entity;

    class Scene
    {
    public:
        Scene(std::string name);
        void Init();
        ~Scene();

        Entity CreateEntity(std::string name);
        Entity CreateEntityWithUUID(UUID id, std::string name);
        //Entity &GetMainCamera();
        void DestroyEntity(Entity entity);
        void DestroyEntityImmidiatly(Entity entity);
        void DeletePendingEntities();

        void OnUpdate(float frameTime);
        void RenderScene(FrameInfo &frameInfo);

        std::unordered_map<UUID, Entity> &GetEntities() { return m_Entities; }
        Ref<VulkanRendererSystem> &GetVulkanRenderer() { return m_RendererSystem; }

        void SetMainCamera(Entity* camera){
            m_MainCamera = camera;
        }
        Entity* GetMainCamera(){
            return m_MainCamera;
        }

    private:
        Entity *m_MainCamera;

    private:
        std::string m_Name;
        entt::registry m_Registry;
        std::unordered_map<UUID, Entity> m_Entities;
        std::unordered_map<UUID, Entity> m_Pending_Delete_Entities;
        Ref<VulkanRendererSystem> m_RendererSystem;

        friend class SceneHierarchy;
        friend class SceneSerializer;
        friend class Entity;
    };
}