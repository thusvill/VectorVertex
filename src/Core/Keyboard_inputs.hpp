#pragma once
#include "../Render/vv_game_object.hpp"
#include "../Render/vv_window.hpp"

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
        float originalSpeed{2.f};
        float speed_multiplier{2.5f};
        float moveSpeed;
        float lookSpeed{1.f};
        bool isClickedOnViewport = false;
    };
} // namespace VectorVertex
