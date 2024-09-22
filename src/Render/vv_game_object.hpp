#pragma once

#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "vv_model.hpp"
#include "vv_material.hpp"
#include "vv_texture.hpp"
#include <vv_uuid.hpp>
#include "Components.hpp"
namespace VectorVertex
{
   

    class VVGameObject
    {
    public:
        using id_t = uint64_t;
        using Map = std::unordered_map<id_t, VVGameObject>;
        static VVGameObject CreateGameObject(std::string name)
        {
            return VVGameObject{name};
        }

        static VVGameObject MakePointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        VVGameObject(const VVGameObject &) = delete;
        VVGameObject &operator=(const VVGameObject &) = delete;
        VVGameObject(VVGameObject &&) = default;
        VVGameObject &operator=(VVGameObject &&) = default;

        const id_t getId() { return uuid.id; }
        IDComponent uuid;
        std::string m_Name;
        std::shared_ptr<VVModel> model{};
        glm::vec3 color{};
        uint64_t material_id;
        TransformComponent transform;
        TextureComponent texture;

        std::unique_ptr<PointLightComponent> point_light = nullptr;

    private:
        VVGameObject(std::string name)
        {
            m_Name = name;
            texture.data = VVTextureLibrary::GetTexture(VVTextureLibrary::default_uuid).data;
            VV_CORE_INFO("GameObject Created with name:{0} on UUID:{1}", name, uuid.id);
        }
    };
} // namespace VectorVertex
