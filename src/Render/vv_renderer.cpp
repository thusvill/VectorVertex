#include "vv_renderer.hpp"
#include <Log.h>
#include <stdexcept>
#include <Assert.h>
namespace VectorVertex
{
    VVRenderer::VVRenderer(VVWindow &window, VVDevice &device)
        : vvWindow{window}, vvDevice{device}
    {

        recreateSwapChain();
        CreateCommandBuffers();
    }

    VVRenderer::~VVRenderer()
    {
        FreeCommandBuffers();
    }

    void VVRenderer::CreateCommandBuffers()
    {
        commandBuffers.resize(VVSwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vvDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vvDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to allocate command buffers");
            throw std::runtime_error("Failed to create commandbuffers");
        }
    }
    void VVRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(vvDevice.device(), vvDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }
    void VVRenderer::recreateSwapChain()
    {
        auto extent = vvWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = vvWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vvDevice.device());
        if (lveSwapChain == nullptr)
        {
            lveSwapChain = std::make_unique<VVSwapChain>(vvDevice, extent);
        }
        else
        {
            std::shared_ptr<VVSwapChain> oldSwapChain = std::move(lveSwapChain);
            lveSwapChain = std::make_unique<VVSwapChain>(vvDevice, extent, oldSwapChain);
            if (!oldSwapChain->compareSwapFormats(*lveSwapChain.get()))
            {
                VV_CORE_ERROR("Swap chain image(or depth) format does not match!");
                throw std::runtime_error("Swap chain image(or depth) format does not match!");
            }
        }
    }

    VkCommandBuffer VVRenderer::BeginFrame()
    {
        // assert(!isFrameStarted && "Can't call EndFrame while frame is in progress!");
        VV_CORE_ASSERT(lveSwapChain != nullptr, "lveSwapChain is nullptr");

        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            VV_CORE_ERROR("Failed to acquire swap chain image!");
            throw std::runtime_error("Failed to acquire swap chain image!");
        }
        isFrameStarted = true;
        auto commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to begin recording command buffer!");
            throw std::runtime_error("Failed to begin recording command buffer!");
        }
        return commandBuffer;
    }
    void VVRenderer::EndFrame()
    {
        // assert(isFrameStarted && "Can't call EndFrame while frame is not in progress!");
        auto commandBuffer = GetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to record command buffer!");
            throw std::runtime_error("Failed to record command buffer!");
        }
        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vvWindow.wasWindowResized())
        {
            vvWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to present swap chain image!");
            throw std::runtime_error("Failed to present swap chain image!");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VVSwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    void VVRenderer::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer)
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
    void VVRenderer::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer)
    {
        assert(isFrameStarted && "Can't call BeginSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = framebuffer;

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
    void VVRenderer::EndSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call EndSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

   

} // namespace VectorVertex
