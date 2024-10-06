#pragma once
#include "vk_window.hpp"

#include <Components.hpp>
namespace VectorVertex
{
    class KeyboardInputs
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow *window, float dt, TransformComponent &transform);

        KeyMappings keys{};
        float originalSpeed{2.5f};
        float speed_multiplier{3.5f};
        float moveSpeed;
        float lookSpeed{1.f};
        bool isClickedOnViewport = false;
    };
} // namespace VectorVertex
