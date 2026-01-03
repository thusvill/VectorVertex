#include "FrameBuffer.hpp"
#include <Render/Renderer.hpp>
#include <Core/Log.h>

#include <Plattform/Vulkan/vk_framebuffer.hpp>

namespace VectorVertex
{
    Ref<FrameBuffer> FrameBuffer::Create(FrameBufferSpecification& specification)
    {
        switch(Renderer::GetAPI()){
            case RendererAPI::API::None:    VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:  return CreateRef<VKFrameBuffer>(specification);

        }
    }

} // namespace VectorVertex
