#pragma once
#include <vvpch.hpp>
#include <Base.h>
#include <Flags.hpp>
namespace VectorVertex
{


    class Buffer
    {
    public:
        virtual void map(uint64_t size,uint64_t offset);
        virtual void unmap();
        virtual void writeToBuffer(void *data,uint64_t size, uint64_t offset);
        virtual void flush(uint64_t size, uint64_t offset);
        virtual void writeToIndex(void *data, int index);
        virtual void flushIndex(int index);

        virtual void* getBuffer();

        static Ref<Buffer> Create(uint64_t instanceSize, uint32_t instanceCount, BufferUsageFlagBits usageFlags, MemoryPropertyFlagBits memoryPropertyFlags,uint64_t minOffsetAlignment = 1);
    };
} // namespace VectorVertex
