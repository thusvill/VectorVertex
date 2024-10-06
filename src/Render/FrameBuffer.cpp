#include "FrameBuffer.hpp"
#include <Renderer.hpp>
#include <Log.h>

#include <vk_framebuffer.hpp>

namespace VectorVertex
{
    Ref<FrameBuffer> FrameBuffer::Create(Extent2D size)
    {
        switch(Renderer::GetAPI()){
            case RendererAPI::API::None:    VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:  return CreateRef<VKFrameBuffer>(size);

        }
    }

} // namespace VectorVertex
