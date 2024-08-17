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
        cleanupImGuiResources();
        cleanupOffscreenResources();
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

    void VVRenderer::createOffscreenResources(uint32_t width, uint32_t height, VkDescriptorPool des_pool)
    {
        offscreen_size.height = height;
        offscreen_size.width = width;
        descriptorPool = des_pool;
        createOffscreenImage(width, height);
        createOffscreenImageView();
        createOffscreenRenderPass();
        createOffscreenFramebuffer(width, height);
    }

    void VVRenderer::createOffscreenImage(uint32_t width, uint32_t height)
    {
        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateImage(vvDevice.device(), &imageCreateInfo, nullptr, &offscreenImage);

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(vvDevice.device(), offscreenImage, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(vvDevice.device(), &memoryAllocateInfo, nullptr, &offscreenImageMemory);
        vkBindImageMemory(vvDevice.device(), offscreenImage, offscreenImageMemory, 0);
    }

    void VVRenderer::createOffscreenImageView()
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = offscreenImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(vvDevice.device(), &imageViewCreateInfo, nullptr, &offscreenImageView);
    }

    void VVRenderer::createOffscreenRenderPass()
    {
         VkAttachmentDescription colorAttachment = {};
          colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
          colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
          colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
          colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
          colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
          colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
          colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
          colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            VkAttachmentReference colorAttachmentRef = {};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount = 1;
            renderPassCreateInfo.pAttachments = &colorAttachment;
            renderPassCreateInfo.subpassCount = 1;
            renderPassCreateInfo.pSubpasses = &subpass;

            vkCreateRenderPass(vvDevice.device(), &renderPassCreateInfo, nullptr, &offscreenRenderPass);
    

        
        if (vkCreateRenderPass(vvDevice.device(), &renderPassCreateInfo, nullptr, &offscreenRenderPass) != VK_SUCCESS)
        {
            VV_CORE_ERROR("failed to create render pass!");
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VVRenderer::createOffscreenFramebuffer(uint32_t width, uint32_t height)
    {
        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = offscreenRenderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &offscreenImageView;
        framebufferCreateInfo.width = width;
        framebufferCreateInfo.height = height;
        framebufferCreateInfo.layers = 1;

        vkCreateFramebuffer(vvDevice.device(), &framebufferCreateInfo, nullptr, &offscreenFramebuffer);
    }

    void VVRenderer::BeginOffscreenRenderpass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call BeginSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        // VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        // commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        VkClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = offscreenRenderPass;
        renderPassBeginInfo.framebuffer = offscreenFramebuffer;
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent.width = offscreen_size.width;
        renderPassBeginInfo.renderArea.extent.height = offscreen_size.height;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
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

    void VVRenderer::EndOffscreenRenderpass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call EndSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
        // renderOffscreen(commandBuffer);
    }

    void VVRenderer::renderOffscreen(VkCommandBuffer commandBuffer)
    {
        // Submit the offscreen rendering command buffer to the graphics queue
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vvDevice.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vvDevice.graphicsQueue());
    }

    void VVRenderer::cleanupOffscreenResources()
    {
        vkDestroyFramebuffer(vvDevice.device(), offscreenFramebuffer, nullptr);
        vkDestroyRenderPass(vvDevice.device(), offscreenRenderPass, nullptr);
        vkDestroyImageView(vvDevice.device(), offscreenImageView, nullptr);
        vkFreeMemory(vvDevice.device(), offscreenImageMemory, nullptr);
        vkDestroyImage(vvDevice.device(), offscreenImage, nullptr);
    }

    void VVRenderer::createImGuiDescriptorSet()
    {
        // Create a sampler for ImGui
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        vkCreateSampler(vvDevice.device(), &samplerCreateInfo, nullptr, &imguiSampler);

        // Create a descriptor set layout for ImGui
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = 0;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = 1;
        layoutCreateInfo.pBindings = &layoutBinding;

        vkCreateDescriptorSetLayout(vvDevice.device(), &layoutCreateInfo, nullptr, &imguiDescriptorSetLayout);

        // Allocate and update the descriptor set
        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool; // Assuming you have a descriptor pool
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &imguiDescriptorSetLayout;

        vkAllocateDescriptorSets(vvDevice.device(), &allocateInfo, &imguiDescriptorSet);

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.sampler = imguiSampler;
        imageInfo.imageView = offscreenImageView; // Offscreen image view
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = imguiDescriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(vvDevice.device(), 1, &descriptorWrite, 0, nullptr);
    }

    void VVRenderer::cleanupImGuiResources()
    {
        vkDestroySampler(vvDevice.device(), imguiSampler, nullptr);
        vkDestroyDescriptorSetLayout(vvDevice.device(), imguiDescriptorSetLayout, nullptr);
    }

    uint32_t VVRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(vvDevice.getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type");
    }

} // namespace VectorVertex
