#pragma once
#include <vvpch.hpp>
#include <Log.h>
#include "vk_window.hpp"
#include "vk_device.hpp"
#include "vk_swap_chain.hpp"

#include <Window.hpp>

namespace VectorVertex
{
    class VKRenderer
    {
    public:
        VKRenderer() = default;
        VKRenderer(Window *window);
        ~VKRenderer();

        VKRenderer(const VKRenderer &) = delete;
        VKRenderer &operator=(const VKRenderer &) = delete;

        VkRenderPass GetSwapchainRenderPass() const
        {
            return m_SwapChain->getRenderPass();
        }
        float GetAspectRatio() const
        {
            return m_SwapChain->extentAspectRatio();
        }

        bool IsFrameInProgress() { return isFrameStarted; }
        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            VV_CORE_ASSERT(isFrameStarted , "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }
        int GetFrameIndex() const
        {
            VV_CORE_ASSERT(isFrameStarted , "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }
        VkCommandBuffer
        BeginFrame();
        void EndFrame();
        void BeginSwapchainRenderPass(VkCommandBuffer commandBuffer);
        void BeginSwapchainRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer);
        void EndSwapchainRenderPass(VkCommandBuffer commandBuffer);
        uint32_t GetSwapchainImageCount() const { return m_SwapChain->getImageCount(); }
        VKSwapChain *Get_Swapchain() const { return m_SwapChain.get(); }
        Window& GetWindow() {return vkWindow;}

        static VKRenderer& Get() {return *s_Instance;}

    private:
        static VKRenderer* s_Instance;
        void
        CreateCommandBuffers();
        void FreeCommandBuffers();

        void recreateSwapChain();

        Window &vkWindow;
        VKDevice &vkDevice;
        std::unique_ptr<VKSwapChain> m_SwapChain;

        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };

} // namespace VectorVertex
