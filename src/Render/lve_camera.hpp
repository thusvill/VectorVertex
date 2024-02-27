#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace lve
{
    class LveCamera
    {
    public:
        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{.0f, -1.0f, 0.0f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{.0f, -1.0f, 0.0f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

        const glm::mat4 &GetProjection() const
        {
            return projectionMatrix;
        }
        const glm::mat4 &GetView() const { return viewMatrix; }

        const glm::mat4 &GetInverseViewMatrix() const { return inverseViewMatrix; }

        const glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }

    private:
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 inverseViewMatrix{1.0f};
    };
} // namespace lve
