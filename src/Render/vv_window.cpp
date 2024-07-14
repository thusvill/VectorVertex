#include "vv_window.hpp"
#include <stdexcept>
#include <Log.h>
namespace VectorVertex
{
    VVWindow::VVWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    VVWindow::~VVWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VVWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            VV_CORE_ERROR("failed to create window surface!");
            throw std::runtime_error("failed to create window surface!");
        }
        //VV_CORE_INFO("Window surface created successfully!");
    }

    void VVWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto vvWindow = reinterpret_cast<VVWindow *>(glfwGetWindowUserPointer(window));
        vvWindow->framebufferResized = true;
        vvWindow->width = width;
        vvWindow->height = height;
    }

    void VVWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }
} // namespace VectorVertex
