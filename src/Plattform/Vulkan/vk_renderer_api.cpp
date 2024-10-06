#include "vk_renderer_api.hpp"

namespace VectorVertex
{
    VKRendererAPI::VKRendererAPI(Window *window) : m_Window(*window)
    {
        VV_CORE_INFO("Vulkan Render API Created!");
    }

    void VKRendererAPI::Init()
    {
        recreateSwapChain();
        CreateCommandBuffers();
    }

    void VKRendererAPI::BeginFrame()
    {
        VV_CORE_ASSERT(m_SwapChain != nullptr, "m_SwapChain is nullptr");

        auto result = m_SwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
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
        };
    }

    void VKRendererAPI::EndFrame()
    {
        auto commandBuffer = GetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to record command buffer!");
            throw std::runtime_error("Failed to record command buffer!");
        }

        auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized())
        {
            m_Window.resetWindowResizedFlag();
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

    void VKRendererAPI::BeginRenderPass()
    {
        auto commandBuffer = GetCurrentCommandBuffer();
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

    void VKRendererAPI::EndRenderPass()
    {
        auto commandBuffer = GetCurrentCommandBuffer();

        assert(isFrameStarted && "Can't call EndSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

    void VKRendererAPI::DrawMesh(MeshData data)
    {
        auto commandBuffer = GetCurrentCommandBuffer();
        std::vector<VkBuffer> buffers;
        for (auto &buffer : data.m_VertexBuffers)
        {
            buffers.push_back(reinterpret_cast<VkBuffer>(buffer.getBuffer()));
        }

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers.data(), offsets);
        if (data.m_IndexBuffer.getBuffer())
        {
            vkCmdBindIndexBuffer(commandBuffer, reinterpret_cast<VkBuffer>(data.m_IndexBuffer.getBuffer()), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, data.m_IndexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, data.m_VertexCount, 1, 0, 0);
        }
    }

    void VKRendererAPI::CreateCommandBuffers()
    {
        auto &vkDevice = VKDevice::Get();
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

    void VKRendererAPI::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(VKDevice::Get().device(), VKDevice::Get().getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void VKRendererAPI::recreateSwapChain()
    {
        auto &vkDevice = VKDevice::Get();
        auto extent = m_Window.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = m_Window.getExtent();
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

} // namespace VectorVertex
