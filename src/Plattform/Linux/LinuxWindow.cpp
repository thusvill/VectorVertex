#include "LinuxWindow.hpp"
#include <MouseEvent.hpp>
#include <KeyEvent.hpp>
#include <ApplicationEvent.hpp>
#include <RendererAPI.hpp>
#include <RenderCommand.hpp>
namespace VectorVertex
{
    static uint8_t s_GLFWWindowCount = 0;

    static void GLFWErrorCallback(int error, const char *description)
    {
        VV_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }
    LinuxWindow::LinuxWindow(const WindowProps &props)
    {
        Init(props);
    }
    LinuxWindow::~LinuxWindow()
    {
        Shutdown();
    }
    void LinuxWindow::OnUpdate()
    {
    }
    void LinuxWindow::SetVSync(bool enabled)
    {
    }
    bool LinuxWindow::IsVSync() const
    {
        return false;
    }
    bool LinuxWindow::wasWindowResized()
    {
        return m_Data.resized;
    }
    void LinuxWindow::resetWindowResizedFlag()
    {
        m_Data.resized = false;
    }
    Extent2D LinuxWindow::getExtent()
    {
        return m_Data.size;
    }
    bool LinuxWindow::shouldClose()
    {
        return glfwWindowShouldClose(m_Window);
    }
    void LinuxWindow::Init(const WindowProps &props)
    {

        m_Data.Title = props.Title;
        m_Data.size.width = props.Width;
        m_Data.size.height = props.Height;

        VV_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (s_GLFWWindowCount == 0)
        {
            int success = glfwInit();
            VV_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        if (RendererAPI::GetRenderAPI() == RendererAPI::API::Vulkan)
        {

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        {
            m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
            GraphicsContext::Get()->Init(this);
            ++s_GLFWWindowCount;
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
                                  {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.size.width = width;
			data.size.height = height;
            data.resized = true;

			WindowResizeEvent event(width, height);
			if(data.EventCallback)
                data.EventCallback(event); });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
                                   {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			if(data.EventCallback)
                data.EventCallback(event); });

        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					if(data.EventCallback)
                        data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					if(data.EventCallback)
                        data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					if(data.EventCallback)
                        data.EventCallback(event);
					break;
				}
			} });

        glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int keycode)
                            {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			if(data.EventCallback)
                data.EventCallback(event); });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
                                   {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					if(data.EventCallback)
                        data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					if(data.EventCallback)
                        data.EventCallback(event);
					break;
				}
			} });

        glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset)
                              {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			if(data.EventCallback)
                data.EventCallback(event); });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xPos, double yPos)
                                 {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
            if(data.EventCallback)
                data.EventCallback(event); });
    }
    void LinuxWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount == 0)
        {
            glfwTerminate();
        }
    }
}