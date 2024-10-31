#pragma once
#include <vulkan/vulkan.h>
#include <Base.h>

namespace VectorVertex
{
    enum class FrameBufferAttachmentType
    {
        None=0,
        RGBA8,
        RGBA8_SNORM,
        RGBA8_UNORM,
        R32_INT,
        Depth
    };
    struct FrameBufferSpecification{
        Extent2D size;
        std::vector<FrameBufferAttachmentType> attachments;
        std::vector<VkClearValue> clearValues;
        bool seperate_renderpass=false;

    };

    class FrameBuffer{
        public:
        virtual void BeginRender()=0;
        virtual void EndRender()=0;
        virtual void Resize(Extent2D size)=0;
        virtual int ReadPixel(uint32_t attachment, int x, int y) = 0;
        virtual Extent2D getViewSize() = 0;
        virtual void* GetFrameBufferImage() =0;

        static Ref<FrameBuffer> Create(FrameBufferSpecification &spec);

    protected:
        std::vector<FrameBufferAttachmentType> m_ColorAttachments;
        FrameBufferAttachmentType m_DepthAttachment = FrameBufferAttachmentType::None;
    };
} // namespace VectorVertex
