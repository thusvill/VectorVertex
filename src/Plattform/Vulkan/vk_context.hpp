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
        VKContext(Window* window);
        virtual ~VKContext() override {}
        virtual void Init() override;
        virtual void SwapBuffers() override;

        virtual void* GetRenderpass() override{

            return RenderCommand::GetRendererAPI()->GetRenderpass();
        }

        virtual void* GetSwapchain() override{
            return RenderCommand::GetRendererAPI()->GetSwapchain();
        }

        private:
        
        Window* m_Window;
        VKDevice m_Device{static_cast<GLFWwindow*>(m_Window->GetNativeWindow())};    
        //VKRenderer m_Renderer{m_Window};

            
    };

} // namespace VectorVertex
