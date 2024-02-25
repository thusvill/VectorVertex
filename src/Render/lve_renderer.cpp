#include "lve_renderer.hpp"

#include <stdexcept>
namespace lve
{
    LveRenderer::LveRenderer(LveWindow &window, LveDevice &device)
        : lveWindow{window}, lveDevice{device}
    {

        recreateSwapChain();
        CreateCommandBuffers();
    }

    LveRenderer::~LveRenderer()
    {
        FreeCommandBuffers();
    }

    void LveRenderer::CreateCommandBuffers()
    {
        commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create commandbuffers");
        }
    }
    void LveRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }
    void LveRenderer::recreateSwapChain()
    {
        auto extent = lveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lveDevice.device());
        if (lveSwapChain == nullptr)
        {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        }
        else
        {
            std::shared_ptr<LveSwapChain> oldSwapChain = std::move(lveSwapChain);
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, oldSwapChain);
            if (!oldSwapChain->compareSwapFormats(*lveSwapChain.get()))
            {
                throw std::runtime_error("Swap chain image(or depth) format does not match!");
            }
                }
    }

    VkCommandBuffer LveRenderer::BeginFrame()
    {
        assert(!isFrameStarted && "Can't call EndFrame while frame is in progress!");

        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }
        isFrameStarted = true;
        auto commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }
        return commandBuffer;
    }
    void LveRenderer::EndFrame()
    {
        assert(isFrameStarted && "Can't call EndFrame while frame is not in progress!");
        auto commandBuffer = GetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized())
        {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image!");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    void LveRenderer::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call BeginSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void LveRenderer::EndSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call EndSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

} // namespace lve
