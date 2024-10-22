#pragma once
#include <Base.h>
#include <Window.hpp>

namespace VectorVertex {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		
		virtual void* GetRenderpass() = 0;
		virtual void* GetSwapchain() = 0;

		static Ref<GraphicsContext> Create(WindowProps& props);

		static Ref<GraphicsContext>& Get(){
			return s_Instance;
		}

		public:
		Scope<Window> m_Window;

		private:
		static Ref<GraphicsContext> s_Instance;
	};

}