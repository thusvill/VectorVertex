#pragma once
#include <Core/vvpch.hpp>
#include <Core/Base.h>
#include <Render/Flags.hpp>
namespace VectorVertex
{

    class Buffer
    {
    public:
        virtual ~Buffer() = default;
        virtual void map(uint64_t size, uint64_t offset) = 0;
        virtual void unmap() = 0;
        virtual void writeToBuffer(void *data, uint64_t size, uint64_t offset) = 0;
        virtual void flush(uint64_t size, uint64_t offset) = 0;
        virtual void writeToIndex(void *data, int index) = 0;
        virtual void flushIndex(int index) = 0;

        virtual VkBuffer getVKBuffer() = 0;

        static Ref<Buffer> Create(uint64_t instanceSize, uint32_t instanceCount, BufferUsageFlagBits usageFlags, MemoryPropertyFlagBits memoryPropertyFlags, uint64_t minOffsetAlignment = 1);
    };
} // namespace VectorVertex
