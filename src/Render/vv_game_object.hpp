#pragma once

#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "vv_model.hpp"
#include "vv_material.hpp"
#include <vv_uuid.hpp>

namespace VectorVertex
{
    struct IDComponent{
        UUID id;
    };

    struct TransformComponent
    {

        glm::vec3 translation{0.0f};
        glm::vec3 scale{1.f};
        glm::vec3 rotation{0.0f};
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };
    struct PointLightComponent
    {
        float light_intensity = 1.0f;
    };

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

        std::unique_ptr<PointLightComponent> point_light = nullptr;

    private:
        VVGameObject(std::string name)
        {
            m_Name = name;
            VV_CORE_INFO("GameObject Created with name:{0} on UUID:{1}", name, uuid.id);
        }
    };
} // namespace VectorVertex
