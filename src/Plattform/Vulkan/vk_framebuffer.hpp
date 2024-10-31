#pragma once
#include <FrameBuffer.hpp>
#include <vulkan/vulkan.h>

namespace VectorVertex
{
    struct VKAttachment
    {
        VkImage image;
        VkImageView imageView;
        VkDeviceMemory imageMemory;
    };
    class VKFrameBuffer : public FrameBuffer
    {
    public:
        VKFrameBuffer(FrameBufferSpecification &spec);
        ~VKFrameBuffer() = default;
        virtual void BeginRender() override;
        virtual void EndRender() override;
        virtual void Resize(Extent2D size) override;
        virtual int ReadPixel(uint32_t attachment, int x, int y) override;
        virtual Extent2D getViewSize() override { return {ViewExtent.width, ViewExtent.height}; }
        virtual void *GetFrameBufferImage() override { return framebufferimageID; }

    private:
        void create_resources();
        VkDeviceMemory allocateImageMemory(VkImage image, VkMemoryPropertyFlags properties);
        VkImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkSampler createSampler();

        VkRenderPass createCustomRenderPass(VkDevice device, const FrameBufferSpecification& spec);


        void clean();
        FrameBufferSpecification m_Specs;
        VkExtent2D ViewExtent{800, 800};
        void *framebufferimageID;
        std::vector<VKAttachment> m_Attachments;
        std::vector<VkImageView> m_ImageViews;
        VkFramebuffer m_Framebuffer;
        VkSampler sampler;

        bool use_seperate_renderpass = false;
       
        VkRenderPass m_CustomRenderpass;

        VkImage depthImage;
        VkImageView depthImageView;
        VkDeviceMemory depthImageMemory;
    };
}