#pragma once
#include <vulkan/vulkan.h>
#include <Base.h>

namespace VectorVertex
{
    enum class FrameBufferFormat
    {
        None = 0,
        RGBA8,
        RGBA16,
        Depth32,
        R32S
    };
    struct FrameBufferSpecification
    {
        Extent2D size;
        std::vector<FrameBufferFormat> attachments;
        int render_image_index = 0;
        bool seperate_renderpass = true;
        bool hasDepth;
    };

    class FrameBuffer
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void BeginRender() = 0;
        virtual void EndRender() = 0;
        virtual void Resize(Extent2D size) = 0;
        virtual Extent2D getViewSize() = 0;
        virtual void *GetFrameBufferImage() = 0;
        virtual void *GetFrameBufferAPI() = 0;

        virtual void *ReadPixel(uint32_t attachment_index, uint32_t x, uint32_t y) = 0;

        virtual FrameBufferSpecification GetSpecification() = 0;

        static Ref<FrameBuffer>
        Create(FrameBufferSpecification &specification);
    };
} // namespace VectorVertex
