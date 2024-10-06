#pragma once
#include <Buffer.hpp>
#include "vk_device.hpp"

namespace VectorVertex
{

    class VKBuffer : public Buffer
    {
    public:
        VKBuffer(
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        ~VKBuffer();
        

        VKBuffer(const VKBuffer &) = delete;
        VKBuffer &operator=(const VKBuffer &) = delete;

        // VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        virtual void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) override;
        virtual void unmap() override;

        virtual void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) override;
        // VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        virtual void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) override;
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        virtual void writeToIndex(void *data, int index) override;
        virtual void flushIndex(int index) override;
        VkDescriptorBufferInfo descriptorInfoForIndex(int index);
        VkResult invalidateIndex(int index);

        virtual void* getBuffer() override{
            return reinterpret_cast<void*>(getBuffer());
        }

        VkBuffer getVKBuffer() const { return buffer; }
        void *getMappedMemory() const { return mapped; }
        uint32_t getInstanceCount() const { return instanceCount; }
        VkDeviceSize getInstanceSize() const { return instanceSize; }
        VkDeviceSize getAlignmentSize() const { return instanceSize; }
        VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
        VkDeviceSize getBufferSize() const { return bufferSize; }

    private:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        VKDevice &vkDevice;
        void *mapped = nullptr;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
    };

} // namespace VectorVertex