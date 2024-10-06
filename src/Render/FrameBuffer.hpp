#pragma once
#include <vulkan/vulkan.h>
#include <Base.h>

namespace VectorVertex
{

    class FrameBuffer{
        public:
        virtual void BeginRender()=0;
        virtual void EndRender()=0;
        virtual void Resize(Extent2D size)=0;
        virtual Extent2D getViewSize() =0;
        virtual void* GetFrameBufferImage() =0;

        Ref<FrameBuffer> Create(Extent2D size);
    };
} // namespace VectorVertex
