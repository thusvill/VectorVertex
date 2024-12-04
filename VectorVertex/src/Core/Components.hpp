#pragma once
#include <vk_texture.hpp>
#include <vk_model.hpp>
#include <vk_camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Render/Material.hpp>
#include <Render/RendererAPI.hpp>

namespace VectorVertex
{

    struct TextureComponent
    {
        TextureComponent()
        {
            m_ID = VVTextureLibrary::GetDefaultTexture();
            VVTextureLibrary::UpdateDescriptors();
            VV_CORE_INFO("Default Texture Loaded {}", m_ID);
        }
        TextureComponent(std::string name, std::string path)
        {
            m_ID = VVTextureLibrary::Create(name, path);
            VV_CORE_INFO("Texture Created {}", m_ID);
        }

        TextureComponent *get()
        {
            return this;
        }

        TextureComponent(const TextureComponent &other)
            : m_ID(other.m_ID) {}

        TextureComponent &operator=(const TextureComponent &other)
        {
            if (this != &other)
            {
                m_ID = other.m_ID;
            }
            return *this;
        }

        uint64_t m_ID;
    };
    struct MaterialComponent
    {

        MaterialComponent()
        {
            m_ID = MaterialLibrary::getDefaultID();
        }
        MaterialComponent(std::string name, MaterialData materialData)
        {
            m_ID = MaterialLibrary::createMaterial(name, materialData);
        }

        MaterialComponent(const MaterialComponent &other)
            : m_ID(other.m_ID) {}

        MaterialComponent &operator=(const MaterialComponent &other)
        {
            if (this != &other)
            {
                m_ID = other.m_ID;
            }
            return *this;
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

        glm::vec3 translation{0.0f}; // Translation remains a vector
        glm::vec3 scale{1.f};        // Scale remains a vector
        glm::vec3 rotation{0.0f};    // Store rotation as Euler angles (in degrees)

        // Internal quaternion for calculations
        glm::quat rotationQuat{1.0f, 0.0f, 0.0f, 0.0f}; // Initialize to identity quaternion

        // Update the quaternion when the Euler angles are set
        void SetRotationEuler(const glm::vec3 &eulerDegrees)
        {
            rotation = eulerDegrees;                              // Set the Euler angles in degrees
            rotationQuat = glm::quat(glm::radians(eulerDegrees)); // Convert to quaternion from degrees
        }

        // Retrieve the current rotation as Euler angles (in degrees)
        glm::vec3 GetRotationEuler() const
        {
            return rotation; // Return the Euler angles directly
        }

        // Retrieve the current rotation as Euler angles (in radians)
        glm::vec3 GetRotationRadians() const
        {
            return glm::radians(rotation); // Convert degrees to radians
        }

        // Set rotation from Euler angles (in radians)
        void SetRotationRadians(const glm::vec3 &eulerRadians)
        {
            SetRotationEuler(glm::degrees(eulerRadians)); // Convert radians to degrees and set
        }

        // Function to get the transformation matrix (with quaternion for rotation)
        glm::mat4 mat4()
        {
            // Convert the quaternion to a 4x4 matrix for rotation
            glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);

            // Combine translation, rotation, and scale into a single transformation matrix
            return glm::translate(glm::mat4(1.0f), translation) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
        }

        // Normal matrix for lighting (uses the inverse transpose of the rotation matrix)
        glm::mat3 normalMatrix()
        {
            // Convert the quaternion to a 3x3 matrix (ignores translation)
            glm::mat3 rotationMatrix = glm::mat3_cast(rotationQuat);

            // Apply scaling inverse (necessary for the normal matrix in lighting calculations)
            const glm::vec3 invScale = 1.0f / scale;

            return glm::transpose(glm::inverse(rotationMatrix)) * glm::mat3(
                                                                      invScale.x, 0.0f, 0.0f,
                                                                      0.0f, invScale.y, 0.0f,
                                                                      0.0f, 0.0f, invScale.z);
        }

        TransformComponent(const TransformComponent &other)
            : translation(other.translation), rotation(other.rotation), scale(other.scale), rotationQuat(other.rotationQuat) {}

        TransformComponent &operator=(const TransformComponent &other)
        {
            if (this != &other)
            {
                translation = other.translation;
                rotation = other.rotation;
                rotationQuat = other.rotationQuat;
                scale = other.scale;
            }
            return *this;
        }
    };

    struct PointLightComponent
    {
        PointLightComponent() = default;
        PointLightComponent(glm::vec3 color, float intensity) : color(color), light_intensity(intensity) {}
        glm::vec3 color = glm::vec3{1.0f};
        float light_intensity = 1.0f;
        float radius = 10.0f;

        PointLightComponent(const PointLightComponent &other)
            : light_intensity(other.light_intensity), color(other.color), radius(other.radius) {}

        PointLightComponent &operator=(const PointLightComponent &other)
        {
            if (this != &other)
            {
                light_intensity = other.light_intensity;
                color = other.color;
                radius = other.radius;
            }
            return *this;
        }
    };

    struct MeshComponent
    {
        MeshComponent() = default;
        MeshComponent(const std::string &filepath)
        {
            m_Model = VKModel::createModelFromFile(filepath);
            path = filepath;
        }
        void UpdateMesh()
        {
            m_Model.reset();
            m_Model = VKModel::createModelFromFile(path);
        }

        MeshData GetMeshData()
        {
            return m_Model->GetMeshData();
        }

        // Custom copy constructor
        MeshComponent(const MeshComponent &other)
            : path(other.path), m_ID(other.m_ID)
        {
            if (other.m_Model)
            {
                m_Model = VKModel::createModelFromFile(other.path); // Clone the model
            }
        }

        // Custom copy assignment operator
        MeshComponent &operator=(const MeshComponent &other)
        {
            if (this == &other)
                return *this;
            path = other.path;
            m_ID = UUID();
            if (other.m_Model)
            {
                m_Model = VKModel::createModelFromFile(other.path); // Clone the model
            }
            else
            {
                m_Model.reset();
            }
            return *this;
        }

        Scope<VKModel> m_Model;
        UUID m_ID;
        std::string path;
    };
    struct CameraComponent
    {
        CameraComponent() = default;
        CameraComponent(bool is_main)
        {
            mainCamera = is_main;
        }

        CameraComponent(const CameraComponent &other)
            : m_Camera(other.m_Camera), mainCamera(false) {}

        CameraComponent &operator=(const CameraComponent &other)
        {
            if (this != &other)
            {
                m_Camera = other.m_Camera;
                mainCamera = false;
            }
            return *this;
        }

        VKCamera m_Camera;
        bool mainCamera;
    };

    template <typename... Component>
    struct ComponentGroup
    {
    };
    using AllComponents =
        ComponentGroup<TransformComponent, MeshComponent, PointLightComponent, CameraComponent,
                       TextureComponent, MaterialComponent>;

} // namespace VectorVertex
