#include "vk_renderer.hpp"
#include <Log.h>
#include <stdexcept>
#include <Assert.h>
#include <vk_texture.hpp>
namespace VectorVertex
{

    VKRenderer* VKRenderer::s_Instance = nullptr;
    
    VKRenderer::VKRenderer(Window *window)
        : vkWindow{*window}, vkDevice{VKDevice::Get()}
    {
        s_Instance = this;
        recreateSwapChain();
        CreateCommandBuffers();
    }

    VKRenderer::~VKRenderer()
    {
        FreeCommandBuffers();
    }

    void VKRenderer::CreateCommandBuffers()
    {
        commandBuffers.resize(VKSwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to allocate command buffers");
            throw std::runtime_error("Failed to create commandbuffers");
        }
    }
    void VKRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(vkDevice.device(), vkDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }
    void VKRenderer::recreateSwapChain()
    {
        auto extent = vkWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = vkWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vkDevice.device());
        if (m_SwapChain == nullptr)
        {
            m_SwapChain = std::make_unique<VKSwapChain>(vkDevice, extent);
        }
        else
        {
            std::shared_ptr<VKSwapChain> oldSwapChain = std::move(m_SwapChain);
            m_SwapChain = std::make_unique<VKSwapChain>(vkDevice, extent, oldSwapChain);
            if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get()))
            {
                VV_CORE_ERROR("Swap chain image(or depth) format does not match!");
                throw std::runtime_error("Swap chain image(or depth) format does not match!");
            }
        }
    }

    VkCommandBuffer VKRenderer::BeginFrame()
    {
        // assert(!isFrameStarted && "Can't call EndFrame while frame is in progress!");
        VV_CORE_ASSERT(m_SwapChain != nullptr, "m_SwapChain is nullptr");

        
        auto result = m_SwapChain->acquireNextImage(&currentImageIndex);

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
    void VKRenderer::EndFrame()
    {
        // assert(isFrameStarted && "Can't call EndFrame while frame is not in progress!");
        auto commandBuffer = GetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to record command buffer!");
            throw std::runtime_error("Failed to record command buffer!");
        }
        
         auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkWindow.wasWindowResized())
        {
            vkWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to present swap chain image!");
            throw std::runtime_error("Failed to present swap chain image!");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VKSwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    void VKRenderer::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call BeginSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void VKRenderer::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer)
    {
        assert(isFrameStarted && "Can't call BeginSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
        renderPassInfo.framebuffer = framebuffer;

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void VKRenderer::EndSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call EndSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

    

   
   

} // namespace VectorVertex
