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
    ImGui::Text("VectorVertex Engine 1.0");
    ImGui::End();

    ImGui::Begin("Material");
    MaterialData _data = VVMaterialLibrary::getMaterial("supra_body").m_MaterialData;
    float col[4];
    col[0] = _data.color.x;
    col[1] = _data.color.y;
    col[2] = _data.color.z;
    col[3] = _data.color.w;
    ImGui::ColorPicker4("supra_body:", col);
    _data.color = glm::vec4(col[0], col[1], col[2], col[3]);
    VVMaterialLibrary::updateMaterial("supra_body", _data);
    //VV_CORE_INFO("supra_body color: {} {} {} {}", col[0], col[1], col[2], col[3]);
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