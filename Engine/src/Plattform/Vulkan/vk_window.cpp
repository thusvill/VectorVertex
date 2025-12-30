#include "vk_window.hpp"
#include <stdexcept>
#include <Log.h>
namespace VectorVertex
{
    VKWindow::VKWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    VKWindow::~VKWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VKWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            VV_CORE_ERROR("failed to create window surface!");
            throw std::runtime_error("failed to create window surface!");
        }
        //VV_CORE_INFO("Window surface created successfully!");
    }

    void VKWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto vkWindow = reinterpret_cast<VKWindow *>(glfwGetWindowUserPointer(window));
        vkWindow->framebufferResized = true;
        vkWindow->width = width;
        vkWindow->height = height;
        
    }

    void VKWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }
} // namespace VectorVertex
