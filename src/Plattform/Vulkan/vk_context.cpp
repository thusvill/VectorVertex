#include "vk_context.hpp"
#include <RenderCommand.hpp>

namespace VectorVertex
{
    VKContext::VKContext(WindowProps &props)
    {
        m_Window = Window::Create(props);
        m_Device = CreateRef<VKDevice>(m_Window->GetNativeWindow());
        VV_CORE_INFO("Vulkan Context Created!");
        Init();
    }

    void VKContext::Init()
    {
        RenderCommand::Init(m_Window.get());
    }

    void VKContext::SwapBuffers()
    {   
    }

} // namespace VectorVertex
