#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
namespace VectorVertex
{
    class VKWindow
    {
    public:
        VKWindow() = default;
        VKWindow(int w, int h, std::string name);
        ~VKWindow();

        VKWindow(const VKWindow &) = delete;
        VKWindow &operator=(const VKWindow &) = delete;
        bool
        shouldClose()
        {
            return glfwWindowShouldClose(window);
        }
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; }

        GLFWwindow *getGLFWwindow() const { return window; }

        void
        createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();
        int width, height;
        bool framebufferResized = false;
        std::string windowName;
        GLFWwindow *window;
    };
}