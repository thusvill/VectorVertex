#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <Core/Log.h>
namespace VectorVertex
{
    class VKCamera
    {

    public:
        enum class ProjectionType
        {
            Orthographic = 0,
            Perspective = 1
        };
        struct CameraData
        {
            ProjectionType m_ProjectionType{1};
            float fov = glm::radians(45.0f);
            float aspect = 16.0f / 9.0f;
            float near = 0.1f;
            float far = 1000.0f;
            float orthoWidth = 10.0f;
            float orthoHeight = 10.0f;
            float orthoNear = -1.0f;
            float orthoFar = 1.0f;
        };
        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetOrthographicProjection(float width, float height, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);
        void SetPerspectiveProjection();

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{.0f, -1.0f, 0.0f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{.0f, -1.0f, 0.0f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
        void RecalculateProjection();
        void Resize(VkExtent2D new_size)
        {
            data.aspect = static_cast<float>(new_size.width) / static_cast<float>(new_size.height);
            SetPerspectiveProjection(data.fov, data.aspect, data.near, data.far);
            VV_CORE_INFO("Camera Resized!");
        }
        void SetProjectionType(ProjectionType type)
        {
            data.m_ProjectionType = type;

            const char *projectionTypeStrings[] = {"Orthographic", "Perspective"};
            VV_CORE_INFO("Camera Projection Changed to : {0}", projectionTypeStrings[(int)type]);
            RecalculateProjection();
        }

        const glm::mat4 &GetProjection() const
        {
            return projectionMatrix;
        }
        const glm::mat4 &GetView() const { return viewMatrix; }

        const glm::mat4 &GetInverseViewMatrix() const { return inverseViewMatrix; }

        const glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }

        const ProjectionType GetProjectionType() const { return data.m_ProjectionType; }

        CameraData &GetCameraData()
        {

            return data;
        }

        void SetCameraData(CameraData data)
        {
            this->data = data;
        }

        CameraData data;
        
    private:
        
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 inverseViewMatrix{1.0f};
    };
} // namespace VectorVertex
