#pragma once
#include <FrameBuffer.hpp>
#include <vulkan/vulkan.h>

namespace VectorVertex
{
    class VKFrameBuffer : public FrameBuffer
    {
    public:
        VKFrameBuffer(Extent2D size);
        ~VKFrameBuffer() = default;
        virtual void BeginRender() override;
        virtual void EndRender() override;
        virtual void Resize(Extent2D size) override;
        virtual Extent2D getViewSize() override { return {ViewExtent.width, ViewExtent.height}; }
        virtual void *GetFrameBufferImage() override {return framebufferimageID;}

    private:
        void create_resources();
        void clean();
        VkExtent2D ViewExtent{800, 800};
        void *framebufferimageID;
        VkImage offscreenImage;
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
        VkDeviceMemory offscreenImageMemory;
        VkImageView offscreenImageView;
        VkFramebuffer offscreenFramebuffer;
        VkSampler sampler;
        VkDescriptorSet descriptorSet;
    };
}