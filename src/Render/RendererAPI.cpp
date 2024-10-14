#include "RendererAPI.hpp"
#include "RendererAPI.hpp"
#include <vk_renderer_api.hpp>

namespace VectorVertex
{
    RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
    
    Scope<RendererAPI> RendererAPI::Create(Window *window)
    {
        switch (GetRenderAPI())
        {
        case RendererAPI::API::None:
            VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateScope<VKRendererAPI>(window);            
        }

        VV_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}