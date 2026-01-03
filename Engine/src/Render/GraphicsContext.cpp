#include "GraphicsContext.hpp"
#include <Core/Base.h>
#include <Render/RendererAPI.hpp>
#include <Plattform/Vulkan/vk_context.hpp>
#include <Render/Window.hpp>
#include <Plattform/Linux/LinuxWindow.hpp>

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
