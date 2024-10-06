#include "vk_framebuffer.hpp"
#include <GraphicsContext.hpp>
#include <vk_renderer.hpp>
#include <vk_swap_chain.hpp>
#include <imgui_impl_vulkan.h>
namespace VectorVertex
{
    VKFrameBuffer::VKFrameBuffer(Extent2D size)
    {
        if (size.height == 0 || size.width == 0)
        {
            size = {800, 800};
        }
        create_resources();
    }

    void VKFrameBuffer::BeginRender()
    {
        auto commandBuffer = VKRenderer::Get().GetCurrentCommandBuffer();
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = reinterpret_cast<VkRenderPass>(GraphicsContext::Get()->GetRenderpass());
        renderPassInfo.framebuffer = offscreenFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = ViewExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.17f, 0.17f, 0.17f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(ViewExtent.width);
        viewport.height = static_cast<float>(ViewExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, ViewExtent};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void VKFrameBuffer::EndRender()
    {
        auto commandBuffer = VKRenderer::Get().GetCurrentCommandBuffer();

        vkCmdEndRenderPass(commandBuffer);

        // Transition image layout for ImGui
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = offscreenImage;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }
    void VKFrameBuffer::Resize(Extent2D size)
    {
        if (size.height == 0 || size.width == 0)
        {
            ViewExtent = {1, 1};
        }

        // SetAccordingtoAspectRatio(ViewExtent, size);
        float newAR = static_cast<float>(ViewExtent.width) / static_cast<float>(ViewExtent.height);
        float AR = static_cast<float>(size.width) / static_cast<float>(size.height);

        if (newAR > AR)
        {
            ViewExtent.height = size.height;
            ViewExtent.width = ViewExtent.height * AR;
        }
        else
        {
            ViewExtent.width = size.width;
            ViewExtent.height = ViewExtent.width / AR;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(VKDevice::Get().getPhysicalDevice(), &deviceProperties);
        uint32_t maxFramebufferWidth = deviceProperties.limits.maxFramebufferWidth;
        uint32_t maxFramebufferHeight = deviceProperties.limits.maxFramebufferHeight;

        // Adjust dimensions if they exceed limits
        ViewExtent.width = std::min(size.width, maxFramebufferWidth);
        ViewExtent.height = std::min(size.height, maxFramebufferHeight);

        VV_CORE_INFO("Extent Resized with WIDTH: {0}, HEIGHT {1} at {2} aspect ratio.", ViewExtent.width, ViewExtent.height, AR);

        vkDeviceWaitIdle(VKDevice::Get().device());
        clean();
        create_resources();
    }
    void VKFrameBuffer::create_resources()
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        imageInfo.extent.width = ViewExtent.width;
        imageInfo.extent.height = ViewExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        vkCreateImage(VKDevice::Get().device(), &imageInfo, nullptr, &offscreenImage);

        // Step 2: Allocate memory for the offscreen image
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(VKDevice::Get().device(), offscreenImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VKDevice::Get().findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(VKDevice::Get().device(), &allocInfo, nullptr, &offscreenImageMemory);
        vkBindImageMemory(VKDevice::Get().device(), offscreenImage, offscreenImageMemory, 0);

        // Step 3: Create the image view for the offscreen image
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = offscreenImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(VKDevice::Get().device(), &viewInfo, nullptr, &offscreenImageView);

        VkImageCreateInfo depthImageInfo = {};
        depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImageInfo.format = reinterpret_cast<VKSwapChain*>(GraphicsContext::Get()->GetSwapchain())->findDepthFormat();
        depthImageInfo.extent.width = ViewExtent.width;
        depthImageInfo.extent.height = ViewExtent.height;
        depthImageInfo.extent.depth = 1;
        depthImageInfo.mipLevels = 1;
        depthImageInfo.arrayLayers = 1;
        depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        vkCreateImage(VKDevice::Get().device(), &depthImageInfo, nullptr, &depthImage);

        // Allocate memory for depth image
        vkGetImageMemoryRequirements(VKDevice::Get().device(), depthImage, &memRequirements);
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VKDevice::Get().findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(VKDevice::Get().device(), &allocInfo, nullptr, &depthImageMemory);
        vkBindImageMemory(VKDevice::Get().device(), depthImage, depthImageMemory, 0);

        // Create the image view for the depth attachment
        VkImageViewCreateInfo depthViewInfo{};
        depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthViewInfo.image = depthImage;
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.format = reinterpret_cast<VKSwapChain*>(GraphicsContext::Get()->GetSwapchain())->findDepthFormat();
        depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthViewInfo.subresourceRange.baseMipLevel = 0;
        depthViewInfo.subresourceRange.levelCount = 1;
        depthViewInfo.subresourceRange.baseArrayLayer = 0;
        depthViewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(VKDevice::Get().device(), &depthViewInfo, nullptr, &depthImageView);

        // Step 5: Create the framebuffer with both color and depth attachments
        std::array<VkImageView, 2> framebufferAttachments = {offscreenImageView, depthImageView};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = VKRenderer::Get().GetSwapchainRenderPass(); // Assuming it is compatible with both color and depth
        framebufferInfo.attachmentCount = static_cast<uint32_t>(framebufferAttachments.size());
        framebufferInfo.pAttachments = framebufferAttachments.data();
        framebufferInfo.width = ViewExtent.width;
        framebufferInfo.height = ViewExtent.height;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(VKDevice::Get().device(), &framebufferInfo, nullptr, &offscreenFramebuffer);

        // Step 6: Create the sampler for the offscreen image
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        vkCreateSampler(VKDevice::Get().device(), &samplerInfo, nullptr, &sampler);

        // Step 7: Create the ImGui texture ID from the offscreen image
        framebufferimageID = ImGui_ImplVulkan_AddTexture(sampler, offscreenImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    void VKFrameBuffer::clean()
    {
        vkDestroySampler(VKDevice::Get().device(), sampler, nullptr);
        vkDestroyFramebuffer(VKDevice::Get().device(), offscreenFramebuffer, nullptr);
        vkDestroyImageView(VKDevice::Get().device(), depthImageView, nullptr);
        vkDestroyImage(VKDevice::Get().device(), depthImage, nullptr);
        vkFreeMemory(VKDevice::Get().device(), depthImageMemory, nullptr);
        vkDestroyImageView(VKDevice::Get().device(), offscreenImageView, nullptr);
        vkDestroyImage(VKDevice::Get().device(), offscreenImage, nullptr);
        vkFreeMemory(VKDevice::Get().device(), offscreenImageMemory, nullptr);
    }
}