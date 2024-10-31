#include "vk_framebuffer.hpp"
#include <GraphicsContext.hpp>
#include <vk_renderer.hpp>
#include <vk_swap_chain.hpp>
#include <imgui_impl_vulkan.h>
#include <RenderCommand.hpp>
#include <vk_api_data.hpp>
namespace VectorVertex
{
    void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Assuming color image; adjust if needed
                                                                         // Set the full range for mip levels and array layers (adjust if using mipmaps or layers)
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            // Transition from undefined layout to transfer source
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            // Transition from color attachment to transfer source layout
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            // Transition back from transfer source to color attachment
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }

        // Insert the image memory barrier into the command buffer
        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,          // Dependency flags
            0, nullptr, // No memory barriers
            0, nullptr, // No buffer memory barriers
            1, &barrier // Image memory barrier count and pointer
        );
    }
    VKFrameBuffer::VKFrameBuffer(FrameBufferSpecification &spec) : m_Specs(spec)
    {

        use_seperate_renderpass = spec.seperate_renderpass;
        if (use_seperate_renderpass)
        {
            m_CustomRenderpass = createCustomRenderPass(VKDevice::Get().device(), spec);
            VulkanAPIData::Get().render_in_seperate_renderpass = use_seperate_renderpass;
            VulkanAPIData::Get().custom_renderpass = &m_CustomRenderpass;
        }

        if (spec.size.height == 0 || spec.size.width == 0)
        {
            spec.size = {800, 800};
        }

        for (auto format : spec.attachments)
        {
            if (format != FrameBufferAttachmentType::Depth && format != FrameBufferAttachmentType::None)
            {
                m_ColorAttachments.push_back(format);
            }
            else if (format == FrameBufferAttachmentType::Depth)
            {
                m_DepthAttachment = format;
            }
        }

        create_resources();
    }

    void VKFrameBuffer::BeginRender()
    {

        auto commandBuffer = reinterpret_cast<VkCommandBuffer>(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        if (!use_seperate_renderpass)
        {
            renderPassInfo.renderPass = reinterpret_cast<VkRenderPass>(GraphicsContext::Get()->GetRenderpass());
        }
        else
        {
            if (m_CustomRenderpass == VK_NULL_HANDLE)
                m_CustomRenderpass = createCustomRenderPass(VKDevice::Get().device(), m_Specs);

            renderPassInfo.renderPass = m_CustomRenderpass;
        }
        renderPassInfo.framebuffer = m_Framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = ViewExtent;

        renderPassInfo.clearValueCount = static_cast<uint32_t>(m_Specs.clearValues.size());
        renderPassInfo.pClearValues = m_Specs.clearValues.data();

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
        auto commandBuffer = reinterpret_cast<VkCommandBuffer>(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());

        vkCmdEndRenderPass(commandBuffer);

        // Transition image layout for ImGui
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_Attachments[0].image;
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
    int VKFrameBuffer::ReadPixel(uint32_t attachment, int x, int y)
    {
        VV_CORE_ASSERT(attachment < m_ColorAttachments.size());

        VkImage enttIDImage = m_Attachments[attachment].image; // Your enttID attachment
        VkFormat imageFormat = VK_FORMAT_R32_SINT;             // Ensure the attachment uses an integer format

        int entityID = 0;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VkDeviceSize bufferSize = sizeof(int);

        // Create staging buffer for reading the pixel
        VKDevice::Get().createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     stagingBuffer, stagingBufferMemory);

        // Begin command buffer
        VkCommandBuffer commandBuffer = VKDevice::Get().beginSingleTimeCommands();

        // Transition the enttID image layout for reading
        transitionImageLayout(commandBuffer, enttIDImage, imageFormat,
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        // Define region for copying a single pixel
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {x, y, 0};
        region.imageExtent = {1, 1, 1}; // Only one pixel

        // Copy pixel to staging buffer
        vkCmdCopyImageToBuffer(commandBuffer, enttIDImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);

        // Revert image layout back
        transitionImageLayout(commandBuffer, enttIDImage, imageFormat,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VKDevice::Get().endSingleTimeCommands(commandBuffer);

        // Map memory to retrieve entity ID
        void *data;
        vkMapMemory(VKDevice::Get().device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(&entityID, data, bufferSize);
        vkUnmapMemory(VKDevice::Get().device(), stagingBufferMemory);

        // Clean up
        vkDestroyBuffer(VKDevice::Get().device(), stagingBuffer, nullptr);
        vkFreeMemory(VKDevice::Get().device(), stagingBufferMemory, nullptr);

        return entityID;
    }

    void VKFrameBuffer::create_resources()
    {
        for (auto &color_attachment : m_ColorAttachments)
        {
            VKAttachment attachment;
            switch (color_attachment)
            {
            case FrameBufferAttachmentType::RGBA8:

                attachment.image = createImage(ViewExtent.width, ViewExtent.height, VK_FORMAT_B8G8R8A8_SRGB,
                                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
                break;
            case FrameBufferAttachmentType::RGBA8_SNORM:

                attachment.image = createImage(ViewExtent.width, ViewExtent.height, VK_FORMAT_B8G8R8A8_SNORM,
                                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
                break;
            case FrameBufferAttachmentType::RGBA8_UNORM:

                attachment.image = createImage(ViewExtent.width, ViewExtent.height, VK_FORMAT_B8G8R8A8_UNORM,
                                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
                break;

            case FrameBufferAttachmentType::R32_INT:

                attachment.image = createImage(ViewExtent.width, ViewExtent.height, VK_FORMAT_R32_SINT,
                                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
                break;
            }

            attachment.imageMemory = allocateImageMemory(attachment.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(VKDevice::Get().device(), attachment.image, attachment.imageMemory, 0);

            switch (color_attachment)
            {
            case FrameBufferAttachmentType::RGBA8:

                attachment.imageView = createImageView(attachment.image, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
                break;
            case FrameBufferAttachmentType::RGBA8_SNORM:

                attachment.imageView = createImageView(attachment.image, VK_FORMAT_B8G8R8A8_SNORM, VK_IMAGE_ASPECT_COLOR_BIT);
                break;
            case FrameBufferAttachmentType::RGBA8_UNORM:

                attachment.imageView = createImageView(attachment.image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
                break;

            case FrameBufferAttachmentType::R32_INT:

                attachment.imageView = createImageView(attachment.image, VK_FORMAT_R32_SINT, VK_IMAGE_ASPECT_COLOR_BIT);
                break;
            }

            m_ImageViews.push_back(attachment.imageView);

            m_Attachments.push_back(attachment);
        }
        if (m_DepthAttachment != FrameBufferAttachmentType::None)
        {
            VkFormat depthFormat = reinterpret_cast<VKSwapChain *>(GraphicsContext::Get()->GetSwapchain())->findDepthFormat();
            depthImage = createImage(ViewExtent.width, ViewExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
            depthImageMemory = allocateImageMemory(depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(VKDevice::Get().device(), depthImage, depthImageMemory, 0);
            depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
            m_ImageViews.push_back(depthImageView);
        }

        std::vector<VkImageView> framebuffer_attachments = {m_ImageViews[0]};
        if (depthImageView)
        {
            framebuffer_attachments.push_back(depthImageView);
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        if (use_seperate_renderpass && m_CustomRenderpass != VK_NULL_HANDLE)
        {
            framebufferInfo.renderPass = m_CustomRenderpass;
        }
        else
        {
            framebufferInfo.renderPass = reinterpret_cast<VkRenderPass>(RenderCommand::GetRendererAPI()->GetRenderpass()); // Assuming it is compatible with both color and depth
        }
        framebufferInfo.attachmentCount = static_cast<uint32_t>(framebuffer_attachments.size());
        framebufferInfo.pAttachments = framebuffer_attachments.data();
        framebufferInfo.width = ViewExtent.width;
        framebufferInfo.height = ViewExtent.height;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(VKDevice::Get().device(), &framebufferInfo, nullptr, &m_Framebuffer);

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
        framebufferimageID = ImGui_ImplVulkan_AddTexture(sampler, m_ImageViews[0], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    VkImage VKFrameBuffer::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent = {width, height, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage image;
        vkCreateImage(VKDevice::Get().device(), &imageInfo, nullptr, &image);
        return image;
    }

    VkDeviceMemory VKFrameBuffer::allocateImageMemory(VkImage image, VkMemoryPropertyFlags properties)
    {
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(VKDevice::Get().device(), image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VKDevice::Get().findMemoryType(memRequirements.memoryTypeBits, properties);

        VkDeviceMemory memory;
        vkAllocateMemory(VKDevice::Get().device(), &allocInfo, nullptr, &memory);
        return memory;
    }

    VkImageView VKFrameBuffer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        vkCreateImageView(VKDevice::Get().device(), &viewInfo, nullptr, &imageView);
        return imageView;
    }

    VkSampler VKFrameBuffer::createSampler()
    {
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

        VkSampler sampler;
        vkCreateSampler(VKDevice::Get().device(), &samplerInfo, nullptr, &sampler);
        return sampler;
    }
    VkRenderPass VKFrameBuffer::createCustomRenderPass(VkDevice device, const FrameBufferSpecification &spec)
    {
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentReference> colorReferences;
        VkAttachmentReference depthReference = {};
        bool hasDepth = false;

        auto getVkFormat = [](FrameBufferAttachmentType type) -> VkFormat
        {
            switch (type)
            {
            case FrameBufferAttachmentType::RGBA8:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case FrameBufferAttachmentType::RGBA8_SNORM:
                return VK_FORMAT_B8G8R8A8_SNORM;
            case FrameBufferAttachmentType::RGBA8_UNORM:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case FrameBufferAttachmentType::R32_INT:
                return VK_FORMAT_R32_SINT;
            case FrameBufferAttachmentType::Depth:
                return VK_FORMAT_D32_SFLOAT;
            default:
                throw std::runtime_error("Unsupported framebuffer attachment type");
            }
        };

        for (size_t i = 0; i < spec.attachments.size(); ++i)
        {
            VkFormat format = getVkFormat(spec.attachments[i]);

            VkAttachmentDescription attachmentDescription = {
                .format = format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = (spec.attachments[i] == FrameBufferAttachmentType::Depth)
                                   ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                   : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            };

            attachmentDescriptions.push_back(attachmentDescription);

            if (spec.attachments[i] == FrameBufferAttachmentType::Depth)
            {
                depthReference = {static_cast<uint32_t>(i), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
                hasDepth = true;
            }
            else
            {
                colorReferences.push_back({static_cast<uint32_t>(i), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
            }
        }

        VkSubpassDescription subpass = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = static_cast<uint32_t>(colorReferences.size()),
            .pColorAttachments = colorReferences.data(),
            .pDepthStencilAttachment = hasDepth ? &depthReference : nullptr,
        };

        VkRenderPassCreateInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size()),
            .pAttachments = attachmentDescriptions.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
        };

        VkRenderPass renderPass;
        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }

        return renderPass;
    }
    void VKFrameBuffer::clean()
    {

        vkDestroySampler(VKDevice::Get().device(), sampler, nullptr);
        vkDestroyFramebuffer(VKDevice::Get().device(), m_Framebuffer, nullptr);
        vkDestroyImageView(VKDevice::Get().device(), depthImageView, nullptr);
        vkDestroyImage(VKDevice::Get().device(), depthImage, nullptr);
        vkFreeMemory(VKDevice::Get().device(), depthImageMemory, nullptr);

        for (auto &attachemnt : m_Attachments)
        {
            vkDestroyImageView(VKDevice::Get().device(), attachemnt.imageView, nullptr);
            vkDestroyImage(VKDevice::Get().device(), attachemnt.image, nullptr);
            vkFreeMemory(VKDevice::Get().device(), attachemnt.imageMemory, nullptr);
        }
        m_Attachments.clear();
        m_ImageViews.clear();
    }
}