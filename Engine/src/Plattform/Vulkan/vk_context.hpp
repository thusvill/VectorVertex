#pragma once
#include <Core/vvpch.hpp>
#include "vk_device.hpp"
#include "vk_renderer.hpp"
#include <Plattform/Linux/LinuxWindow.hpp>
#include <Render/GraphicsContext.hpp>
#include <Render/RenderCommand.hpp>

namespace VectorVertex
{
    class VKContext : public GraphicsContext
    {
    public:
        VKContext();
        virtual ~VKContext() override {}
        virtual void Init(Window *window) override;
        virtual void SwapBuffers() override;

        virtual void *GetRenderpass() override
        {

            return RenderCommand::GetRendererAPI()->GetRenderpass();
        }

        virtual void *GetSwapchain() override
        {
            return RenderCommand::GetRendererAPI()->GetSwapchain();
        }

    private:
        Ref<VKDevice> m_Device;
    };

} // namespace VectorVertex
