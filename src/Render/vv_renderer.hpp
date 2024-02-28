#pragma once

#include "vv_window.hpp"
#include "vv_device.hpp"
#include "vv_swap_chain.hpp"

#include <memory>
#include <vector>
#include <array>
#include <cassert>

namespace VectorVertex
{
    class VVRenderer
    {
    public:
        VVRenderer() = default;
        VVRenderer(VVWindow &window, VVDevice &device);
        ~VVRenderer();

        VVRenderer(const VVRenderer &) = delete;
        VVRenderer &operator=(const VVRenderer &) = delete;

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

        VVWindow &vvWindow;
        VVDevice &vvDevice;
        std::unique_ptr<VVSwapChain> lveSwapChain;

        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
} // namespace VectorVertex
