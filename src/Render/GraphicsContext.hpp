#pragma once
#include <Base.h>

namespace VectorVertex {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		
		virtual void* GetRenderpass() = 0;
		virtual void* GetSwapchain() = 0;

		static Ref<GraphicsContext> Create(void* window);

		static Ref<GraphicsContext>& Get(){
			return s_Instance;
		}

		private:
		static Ref<GraphicsContext> s_Instance;
	};

}