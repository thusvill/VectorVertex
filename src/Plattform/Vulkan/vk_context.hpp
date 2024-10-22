#pragma once
#include <vvpch.hpp>
#include "vk_device.hpp"
#include "vk_renderer.hpp"
#include <Linux/LinuxWindow.hpp>
#include <GraphicsContext.hpp>
#include <RenderCommand.hpp>

namespace VectorVertex
{
    class VKContext : public GraphicsContext
    {
        public:
            VKContext(WindowProps &props);
            virtual ~VKContext() override {}
            virtual void Init() override;
            virtual void SwapBuffers() override;

            virtual void *GetRenderpass() override
            {

                return RenderCommand::GetRendererAPI()->GetRenderpass();
        }

        virtual void* GetSwapchain() override{
            return RenderCommand::GetRendererAPI()->GetSwapchain();
        }

        private:
        
        Ref<VKDevice> m_Device;    
        //VKRenderer m_Renderer{m_Window};

            
    };

} // namespace VectorVertex
