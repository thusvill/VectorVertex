#pragma once
#include <GLFW/glfw3.h>
#include <vvpch.hpp>
#include <Base.h>
#include <Event.hpp>
namespace VectorVertex
{

    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string &title = "Hazel Engine",
                    uint32_t width = 1600,
                    uint32_t height = 900)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual Extent2D getExtent() = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn &callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual bool shouldClose() = 0;

        virtual GLFWwindow *GetNativeWindow() = 0;

        virtual bool wasWindowResized() = 0;
        virtual void resetWindowResizedFlag() = 0;

        static Scope<Window> Create(const WindowProps &props = WindowProps());
    };

}