#include "Application.hpp"
#include "Log.h"
#include <Editor_Layer.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>

namespace VectorVertex
{
    Application *Application::s_Instance = nullptr;

    VkImageView CreateColorAttachmentImageView(VkDevice device, VkImage image, VkFormat format);
    VkImage CreateImage(VKDevice &VKDevice, VkImageUsageFlags usage, uint32_t width, uint32_t height);
    void TransitionImageLayout(VKDevice *VKDevice, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool hasStencilComponent(VkFormat format);

    Application::Application(ProjectInfo &info) : WIDTH(info.width), HEIGHT(info.height), project_name(info.title)
    {
        VV_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        VV_CORE_WARN("Application is Started!");
        VV_CORE_WARN("Initializing ...");

        editor_layer = new EditorLayer(info);

        layers.PushLayer(editor_layer);
        editor_layer->SetupImgui();
        VV_CORE_WARN("Initialized!");
        WindowResizeEvent e(info.width, info.height);
        VV_TRACE(e.ToString());
    }

    Application::~Application()
    {
    }

    void Application::run()
    {

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (m_Running && !VKWindow.shouldClose())
        {
            layers.UpdateAll();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            VVTextureLibrary::UpdateDescriptors();

            glfwPollEvents();

            if (auto commandBuffer = renderer.BeginFrame())
            {

                int frame_index = renderer.GetFrameIndex();
                std::unordered_map<int, VkDescriptorSet> descriptor_sets;
                FrameInfo frameInfo{
                    frame_index,
                    frameTime,
                    commandBuffer};

                editor_layer->OnRender(frameInfo);
                // Main Window Renderer
                {
                    renderer.BeginSwapchainRenderPass(commandBuffer);
                    editor_layer->OnImGuiRender(frameInfo);
                    renderer.EndSwapchainRenderPass(commandBuffer);
                }

                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(VKDevice.device());
    }

} // namespace VectorVertex
