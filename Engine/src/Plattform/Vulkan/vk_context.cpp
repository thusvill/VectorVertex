#include "vk_context.hpp"
#include <RenderCommand.hpp>
#include <Application.hpp>

namespace VectorVertex
{
    VKContext::VKContext()
    {

        VV_CORE_INFO("Vulkan Context Created!");
    }

    void VKContext::Init(Window *window)
    {
        m_Device = CreateRef<VKDevice>(window->GetNativeWindow());
        RenderCommand::Init(window);
    }

    void VKContext::SwapBuffers()
    {
    }

} // namespace VectorVertex
