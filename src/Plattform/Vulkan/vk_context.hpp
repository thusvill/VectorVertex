#pragma once
#include <vvpch.hpp>
#include "vk_device.hpp"
#include "vk_renderer.hpp"
#include <Linux/LinuxWindow.hpp>
#include <GraphicsContext.hpp>

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
             return reinterpret_cast<void*>(m_Renderer.GetSwapchainRenderPass());
        }

        virtual void* GetSwapchain() override{
            return static_cast<void*>(m_Renderer.Get_Swapchain());
        }

        private:
        
        Window* m_Window;
        VKDevice m_Device{static_cast<GLFWwindow*>(m_Window->GetNativeWindow())};    
        VKRenderer m_Renderer{m_Window};

            
    };

} // namespace VectorVertex
