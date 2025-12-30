#pragma once
#include <FrameBuffer.hpp>
#include <vk_renderer.hpp>
#include <vulkan/vulkan.h>

namespace VectorVertex
{
    VkFormat getVKFormat(FrameBufferFormat format);
    struct VKFrameBufferImageAttachment
    {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
    };
    class VKFrameBuffer : public FrameBuffer
    {
    public:
        VKFrameBuffer(FrameBufferSpecification &specification);
        ~VKFrameBuffer() = default;

        virtual void Bind() override;
        virtual void Unbind() override;
        virtual void BeginRender() override;
        virtual void EndRender() override;
        virtual void Resize(Extent2D size) override;
        virtual Extent2D getViewSize() override { return {m_Specification.size.width, m_Specification.size.height}; }
        virtual void *GetFrameBufferImage() override { return framebufferimageID; }
        virtual void *GetFrameBufferAPI() override
        {
            return this;
        }
        virtual FrameBufferSpecification GetSpecification() override
        {
            return m_Specification;
        }
        virtual void *ReadPixel(uint32_t attachment_index, uint32_t x, uint32_t y) override;
        VkRenderPass &getRenderpass() { return m_Renderpass; }

    private:
        FrameBufferSpecification m_Specification;
        void create_resources();
        void clean();
        void *framebufferimageID;
        std::vector<VKFrameBufferImageAttachment> color_attachments{};
        VKFrameBufferImageAttachment depth_atachment{};

        VkFramebuffer m_Framebuffer;
        VkSampler sampler;
        VkDescriptorSet descriptorSet;

    private:
        VkRenderPass m_Renderpass;
        std::vector<VkFormat> colorFormats;
        VkFormat depthFormat;
        void CreateRenderpass();
        void BeginRenderpass();
        void EndRenderpass();
    };
}
