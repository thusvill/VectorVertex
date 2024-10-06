#pragma once
#include <Window.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GraphicsContext.hpp>

namespace VectorVertex
{
	class LinuxWindow : public Window
	{
	public:
		LinuxWindow(const WindowProps &props);
		virtual ~LinuxWindow();
		
		virtual void OnUpdate() override;
		virtual unsigned int GetWidth() const override { return m_Data.size.width; }
		virtual unsigned int GetHeight() const override { return m_Data.size.height; }
		virtual void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;
		virtual bool wasWindowResized() override;
		virtual void resetWindowResizedFlag() override;

		virtual void *GetNativeWindow() const { return m_Window; }

	private:
		virtual void Init(const WindowProps &props);
		virtual void Shutdown();

	private:
		GLFWwindow *m_Window;
		Ref<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			Extent2D size;
			bool VSync;
			bool resized = false;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}