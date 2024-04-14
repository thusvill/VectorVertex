#include "Editor_Layer.hpp"

namespace VectorVertex
{
EditorLayer::EditorLayer() :Layer("EditorLayer")
{
    VV_CORE_INFO("[Layer]:EditorLayer Created!");
}

void EditorLayer::SetupImgui(VVDevice* vv_device, VVRenderer* vv_renderer, VVWindow* vv_window)
{
    VkInstance instance = vv_device->getInstance();
    VV_CORE_ASSERT(instance, "Vulkan Instance should not be null!");

    ImguiConfig imguiConfig;
    imguiConfig.instance = instance; // Assign Vulkan instance handle
    imguiConfig.Device = vv_device->device();
    imguiConfig.renderer = vv_renderer;
    imguiConfig.renderPass = vv_renderer->GetSwapchainRenderPass();
    imguiConfig.PhysicalDevice = vv_device->getPhysicalDevice();
    imguiConfig.graphicsQueue = vv_device->graphicsQueue();
    imguiConfig.imageCount = static_cast<uint32_t>(vv_renderer->GetSwapchainImageCount());

    imgui_layer.InitializeImgui(imguiConfig, vv_window->getGLFWwindow());

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void EditorLayer::OnAttach()
{
}

void EditorLayer::OnUpdate()
{
}

void EditorLayer::OnImGuiRender(FrameInfo &frameInfo)
{
    imgui_layer.Begin();

    ImGui::Begin("Viewport");
    ImGui::Text("Viewport");

    ImGui::End();

    imgui_layer.End(frameInfo.command_buffer);
}




void EditorLayer::OnRender(FrameInfo &frameInfo)
{

    
}

void EditorLayer::OnDetach()
{
}


} // namespace VectorVertex