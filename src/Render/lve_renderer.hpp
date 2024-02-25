#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

#include <memory>
#include <vector>
#include <array>
#include <cassert>

namespace lve
{
    class LveRenderer
    {
    public:
        LveRenderer(LveWindow &window, LveDevice &device);
        ~LveRenderer();

        LveRenderer(const LveRenderer &) = delete;
        LveRenderer &operator=(const LveRenderer &) = delete;

        VkRenderPass GetSwapchainRenderPass() const
        {
            return lveSwapChain->getRenderPass();
        }
        float GetAspectRatio() const
        {
            return lveSwapChain->extentAspectRatio();
        }

        bool IsFrameInProgress() { return isFrameStarted; }
        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }
        int GetFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }
        VkCommandBuffer
        BeginFrame();
        void EndFrame();
        void BeginSwapchainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapchainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void
        CreateCommandBuffers();
        void FreeCommandBuffers();

        void recreateSwapChain();

        LveWindow &lveWindow;
        LveDevice &lveDevice;
        std::unique_ptr<LveSwapChain> lveSwapChain;

        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
} // namespace lve
