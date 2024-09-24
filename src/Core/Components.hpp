#pragma once
#include <vv_texture.hpp>
#include <vv_model.hpp>
#include <vv_camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vv_material.hpp>

namespace VectorVertex
{
    struct TextureComponent
    {
        TextureComponent()
        {
            m_ID = VVTextureLibrary::default_uuid;
            VV_CORE_INFO("Texture Created {}", m_ID);
        }
        TextureComponent(VVDevice &device, std::string name, std::string path)
        {
            m_ID = VVTextureLibrary::Create(device, name, path);
            VV_CORE_INFO("Texture Created {}", m_ID);
        }
        uint64_t m_ID;
    };
    struct MaterialComponent
    {

        MaterialComponent()
        {
            m_ID = VVMaterialLibrary::getDefaultID();
        }
        MaterialComponent(std::string name, MaterialData materialData)
        {
            m_ID = VVMaterialLibrary::createMaterial(name, materialData);
        }
        uint64_t m_ID;
    };
    struct IDComponent
    {
        IDComponent() = default;
        IDComponent(std::string _name) : m_Name(_name) {}
        std::string m_Name;
        UUID id;
    };

    struct TransformComponent
    {
        TransformComponent() = default;

        glm::vec3 translation{0.0f};
        glm::vec3 scale{1.f};
        glm::vec3 rotation{0.0f};
        glm::mat4 mat4()
        {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                {translation.x, translation.y, translation.z, 1.0f}};
        }
        glm::mat3 normalMatrix()
        {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);

            const glm::vec3 invScale = 1.0f / scale;

            return glm::mat3{
                {
                    invScale.x * (c1 * c3 + s1 * s2 * s3),
                    invScale.x * (c2 * s3),
                    invScale.x * (c1 * s2 * s3 - c3 * s1),
                },
                {
                    invScale.y * (c3 * s1 * s2 - c1 * s3),
                    invScale.y * (c2 * c3),
                    invScale.y * (c1 * c3 * s2 + s1 * s3),
                },
                {
                    invScale.z * (c2 * s1),
                    invScale.z * (-s2),
                    invScale.z * (c1 * c2),
                }};
        }
    };

    struct PointLightComponent
    {
        PointLightComponent() = default;
        PointLightComponent(glm::vec3 color, float intensity) : color(color), light_intensity(intensity) {}
        glm::vec3 color = glm::vec3{1.0f};
        float light_intensity = 1.0f;
        float radius = 10.0f;
    };
    struct MeshComponent
    {
        MeshComponent() = default;
        MeshComponent(VVDevice &device, const std::string &filepath)
        {
            m_Model = VVModel::createModelFromFile(device, filepath);
            path = filepath;
        }
        void UpdateMesh(VVDevice &device){
            m_Model.reset();
            m_Model = VVModel::createModelFromFile(device, path);
        }
        Scope<VVModel> m_Model;
        std::string path;
    };
    struct CameraComponent
    {
        CameraComponent() = default;

        VVCamera m_Camera;
    };
} // namespace VectorVertex
