#include "Buffer.hpp"
#include <Renderer.hpp>
#include <vk_buffer.hpp>


namespace VectorVertex
{
    Ref<Buffer> Buffer::Create(uint64_t instanceSize, uint32_t instanceCount, BufferUsageFlagBits usageFlags, MemoryPropertyFlagBits memoryPropertyFlags,uint64_t minOffsetAlignment)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateRef<VKBuffer>(instanceSize, instanceCount,(VkBufferUsageFlags)usageFlags,(VkMemoryPropertyFlags)usageFlags, minOffsetAlignment);
        }
    }
}