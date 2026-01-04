#include "Scene.hpp"
#include "Entity.hpp"
#include <entt/entt.hpp>
#include <Core/Log.h>
#include <Plattform/Vulkan/vk_frame_info.hpp>
#include <Render/RenderCommand.hpp>
#include <Plattform/Vulkan/vk_api_data.hpp>
#include <Core/Application.hpp>
#include <type_traits>

namespace VectorVertex
{

    Scene::Scene(std::string name) : m_Name(name)
    {
        VV_CORE_INFO("Scene {} Created!", name.c_str());
        // m_RendererSystem = CreateRef<VulkanRendererSystem>();
    }
    void Scene::Init()
    {
        // RenderCommand::ClearResources();
    }

    template <typename... Component>
    static void CopyComponent(entt::registry &dst, entt::registry &src, const std::unordered_map<UUID, entt::entity> &enttMap)
    {
        ([&]()
         {
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			} }(), ...);
    }

    template <typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry &dst, entt::registry &src, const std::unordered_map<UUID, entt::entity> &enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template <typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        ([&]()
         {
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>()); }(), ...);
    }

    template <typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
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
    Entity Scene::Duplicate(Entity source)
    {
        std::string name = source.GetComponent<IDComponent>().m_Name;
        Entity newEntity = CreateEntity(name);
        CopyComponentIfExists(AllComponents{}, newEntity, source);
        return newEntity;
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
    void Scene::OnUpdate()
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

        m_Camera.GetCameraData().aspect = static_cast<float>(m_ViewportSize.width) / static_cast<float>(m_ViewportSize.height); // renderer.GetAspectRatio();
        // camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
        if (m_Camera.GetProjectionType() == VKCamera::ProjectionType::Perspective)
        {
            // m_Camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 100.f);
            m_Camera.SetPerspectiveProjection();
        }
        GlobalUBO ubo{};
        ubo.view = m_MainCamera->GetComponent<CameraComponent>().m_Camera.GetView();
        ubo.projection = m_MainCamera->GetComponent<CameraComponent>().m_Camera.GetProjection();
        ubo.inverse_view_matrix = m_MainCamera->GetComponent<CameraComponent>().m_Camera.GetInverseViewMatrix();

        FrameInfo frameInfo{
            RenderCommand::GetRendererAPI()->GetCurrentFrameIndex(),
            Application::Get().GetFrameTime(), RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer(), ubo};

        RenderCommand::UpdateObjects(m_Entities, m_MainCamera, frameInfo);

        VulkanAPIData::Get().m_ubo_buffers[frameInfo.frame_index]->writeToBuffer(&ubo);
        VulkanAPIData::Get().m_ubo_buffers[frameInfo.frame_index]->flush();

        // m_RendererSystem->OnUpdate(frameTime, m_MainCamera);
    }

    void Scene::RenderScene(FrameInfo &frameInfo)
    {

        RenderCommand::DrawScene(m_Entities, frameInfo);
    }
    Entity *Scene::GetMainCamera()
    {
        if (!m_MainCamera->GetComponent<CameraComponent>().mainCamera)
        {
            m_MainCamera = nullptr;
            FindANewCamera();
        }

        return m_MainCamera;
    }
    void Scene::FindANewCamera()
    {
        for (auto &kv : m_Entities)
        {
            auto &obj = kv.second;
            if (obj.GetComponent<CameraComponent>().mainCamera)
            {
                m_MainCamera = &kv.second;
                break;
            }
        }
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T &component)
    {

        static_assert(sizeof(T) == 0);
    }
    template <>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent &component)
    {
        
    }

    template <>
    void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent &component)
    {
        
    }

    template <>
    void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent &component)
    {
        
    }

    template <>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent &component)
    {
        
    }

    template <>
    void Scene::OnComponentAdded<TextureComponent>(Entity entity, TextureComponent &component)
    {
        
    }

    template <>
    void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent &component)
    {
        
    }

    // template <>
    // void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent &component)
    // {
        
        
    // }
}