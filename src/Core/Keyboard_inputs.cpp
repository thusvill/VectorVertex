#include "Keyboard_inputs.hpp"
#include <iostream>

namespace VectorVertex
{
    void KeyboardInputs::moveInPlaneXZ(GLFWwindow *window, float dt, LveGameObject &gameObject)
    {
        moveSpeed = originalSpeed;

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            moveSpeed = originalSpeed * speed_multiplier;
        }

        glm::vec3 rotate{0};
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
            rotate.y += 1.f;
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
            rotate.y -= 1.f;
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
            rotate.x += 1.f;
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
            rotate.x -= 1.f;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            // Get mouse delta movement
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Calculate the change in mouse position
            static double lastMouseX = mouseX;
            static double lastMouseY = mouseY;
            double deltaX = mouseX - lastMouseX;
            double deltaY = mouseY - lastMouseY;

            // Adjust rotation based on mouse movement
            const float sensitivity = 0.5f;
            rotate.y += static_cast<float>(deltaX) * sensitivity;
            rotate.x += static_cast<float>(-deltaY) * sensitivity;

            // Update last mouse position
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        // limit pitch values between about +/- 85ish degrees
        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
            moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
            moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
            moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
            moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
            moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
            moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
        // std::cout << "Camera | position x:" << gameObject.transform.translation.x << " y:" << gameObject.transform.translation.y << " z:" << gameObject.transform.translation.z << "\n Camera| rotation x:" << gameObject.transform.rotation.x << " y:" << gameObject.transform.rotation.y << " z:" << gameObject.transform.rotation.z << std::endl;
        // std::cout<<moveDir.x<<" "<<moveDir.y<<" "<<moveDir.z<<std::endl;
    }

} // namespace VectorVertex
