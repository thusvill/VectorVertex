#include "GraphicsContext.hpp"
#include <Base.h>
#include <RendererAPI.hpp>
#include <vk_context.hpp>
#include <Window.hpp>
#include <Linux/LinuxWindow.hpp>

namespace VectorVertex
{
	Ref<GraphicsContext> GraphicsContext::s_Instance = nullptr;

	Ref<GraphicsContext> GraphicsContext::Create()
	{
		switch (RendererAPI::GetRenderAPI())
		{
		case RendererAPI::API::None:
			VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			auto gc = CreateRef<VKContext>();
			s_Instance = gc;
			return gc;
		}

		VV_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
} // namespace VectorVertex
