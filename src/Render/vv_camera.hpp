#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <Log.h>
namespace VectorVertex
{
    class VVCamera
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
            float fov, aspect, near, far;
            float orthoWidth, orthoHeight, orthoNear, orthoFar;
        };
        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetOrthographicProjection(float width, float height, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{.0f, -1.0f, 0.0f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{.0f, -1.0f, 0.0f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
        void RecalculateProjection();
        void Resize(VkExtent2D new_size)
        {
            aspect = static_cast<float>(new_size.width) / static_cast<float>(new_size.height);
            SetPerspectiveProjection(fov, aspect, near, far);
            VV_CORE_INFO("Camera Resized!");
        }
        void SetProjectionType(ProjectionType type)
        {
            m_ProjectionType = type;

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

        const ProjectionType GetProjectionType() const { return m_ProjectionType; }

        const CameraData GetCameraData() const {
            CameraData data;
            data.m_ProjectionType = m_ProjectionType;
            data.fov = fov;
            data.near = near;
            data.far = far;
            data.aspect = aspect;
            data.orthoFar = orthoFar;
            data.orthoNear = orthoNear;
            data.orthoHeight = orthoHeight;
            data.orthoWidth = orthoWidth;

            return data;
            
        }

        void SetCameraData(CameraData data){
             m_ProjectionType = data.m_ProjectionType;
             fov = data.fov;
             near = data.near;
             far = data.far;
             aspect = data.aspect;
             orthoFar = data.orthoFar;
             orthoNear = data.orthoNear;
             orthoHeight = data.orthoHeight;
             orthoWidth = data.orthoWidth;
        }

    private:
        ProjectionType m_ProjectionType{1};
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 inverseViewMatrix{1.0f};
        float fov, aspect, near, far;
        float orthoWidth, orthoHeight, orthoNear, orthoFar;
    };
} // namespace VectorVertex
