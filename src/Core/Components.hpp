#pragma once
#include <vv_texture.hpp>
#include <vv_model.hpp>
#include <glm/glm.hpp>

namespace VectorVertex
{
    struct TextureComponent
    {
        TextureData data;
    };
    struct IDComponent
    {
        IDComponent() = default;
        IDComponent(std::string _name): m_Name(_name){}
        std::string m_Name;
        UUID id;
    };

    struct TransformComponent
    {
        TransformComponent()=default;

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
    struct MeshComponent
    {

        VVModel m_Model;
    };
} // namespace VectorVertex
