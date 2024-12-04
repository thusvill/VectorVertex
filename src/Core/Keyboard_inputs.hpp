#pragma once
<<<<<<<< HEAD:VectorVertex/src/Core/Keyboard_inputs.hpp
#include "vvpch.hpp"
#include <Plattform/Vulkan/vk_window.hpp>

#include "Components.hpp"
========
#include <vvpch.hpp>
#include "vk_window.hpp"

#include <Components.hpp>
>>>>>>>> 400cd19c24c23a0b77a9a6741370ff57828b1ef2:src/Core/Keyboard_inputs.hpp

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
        bool isMoving() {return is_moving;}

        KeyMappings keys{};
        float originalSpeed{3.5f};
        float speed_multiplier{3.5f};
        float moveSpeed;
        float lookSpeed{1.f};
        bool is_moving = false;
        bool isClickedOnViewport = false;
    };
} // namespace VectorVertex
