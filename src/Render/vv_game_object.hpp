#pragma once

#include <memory>
#include <unordered_map>
#include "vv_model.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace VectorVertex
{

    struct TransformComponent
    {

        glm::vec3 translation{};
        glm::vec3 scale{1.f};
        glm::vec3 rotation;
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
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, VVGameObject>;
        static VVGameObject CreateGameObject()
        {
            static id_t current_id = 0;
            return VVGameObject{current_id++};
        }

        static VVGameObject MakePointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        VVGameObject(const VVGameObject &) = delete;
        VVGameObject &operator=(const VVGameObject &) = delete;
        VVGameObject(VVGameObject &&) = default;
        VVGameObject &operator=(VVGameObject &&) = default;

        const id_t getId() { return id; }

        std::shared_ptr<VVModel> model{};
        glm::vec3 color{};
        TransformComponent transform;

        std::unique_ptr<PointLightComponent> point_light = nullptr;

    private:
        VVGameObject(id_t id) : id{id}
        {
        }
        id_t id;
    };
} // namespace VectorVertex
