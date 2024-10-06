#pragma once

#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "vk_model.hpp"
#include "vk_material.hpp"
#include "vk_texture.hpp"
#include <UUID.hpp>
#include "Components.hpp"
namespace VectorVertex
{
   

    class VKGameObject
    {
    public:
        using id_t = uint64_t;
        using Map = std::unordered_map<id_t, VKGameObject>;
        static VKGameObject CreateGameObject(std::string name)
        {
            return VKGameObject{name};
        }

        static VKGameObject MakePointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        VKGameObject(const VKGameObject &) = delete;
        VKGameObject &operator=(const VKGameObject &) = delete;
        VKGameObject(VKGameObject &&) = default;
        VKGameObject &operator=(VKGameObject &&) = default;

        const id_t getId() { return uuid.id; }
        IDComponent uuid;
        std::string m_Name;
        std::shared_ptr<VKModel> model{};
        glm::vec3 color{};
        uint64_t material_id;
        TransformComponent transform;
        TextureComponent texture;

        std::unique_ptr<PointLightComponent> point_light = nullptr;

    private:
        VKGameObject(std::string name)
        {
            m_Name = name;
            texture.m_ID = VVTextureLibrary::GetDefaultTexture();
            VV_CORE_INFO("GameObject Created with name:{0} on UUID:{1}", name, uuid.id);
        }
    };
} // namespace VectorVertex
