#include "vk_framebuffer.hpp"
#include <Render/GraphicsContext.hpp>
#include <Plattform/Vulkan/vk_swap_chain.hpp>
#include <imgui_impl_vulkan.h>
#include <Render/RenderCommand.hpp>
#include <Plattform/Vulkan/vk_api_data.hpp>
namespace VectorVertex
{
    VkFormat getVKFormat(FrameBufferFormat format)
    {
        switch (format)
        {
        case FrameBufferFormat::RGBA8:
            return VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case FrameBufferFormat::RGBA16:
            return VK_FORMAT_R16G16B16A16_SNORM;
            break;
        case FrameBufferFormat::Depth32:
            return VKDevice::Get().findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            break;
        case FrameBufferFormat::R32S:
            return VK_FORMAT_R32_SINT;
            break;
        }
    }

    VKFrameBuffer::VKFrameBuffer(FrameBufferSpecification &specification) : m_Specification(specification)
    {
        if (specification.size.height == 0 || specification.size.width == 0)
        {
            m_Specification.size = {800, 800};
        }
        create_resources();
    }

    void VKFrameBuffer::Bind()
    {
        BeginRenderpass();
    }

    void VKFrameBuffer::Unbind()
    {
        EndRenderpass();
    }

    void VKFrameBuffer::BeginRender()
    {
        auto commandBuffer = reinterpret_cast<VkCommandBuffer>(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());

        // Transition from SHADER_READ_ONLY_OPTIMAL to COLOR_ATTACHMENT_OPTIMAL
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // Changed from SHADER_READ_ONLY_OPTIMAL
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = color_attachments[m_Specification.render_image_index].image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (!m_Specification.seperate_renderpass)

        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = *reinterpret_cast<VkRenderPass *>(RenderCommand::GetRendererAPI()->GetRenderpass());
            renderPassInfo.framebuffer = m_Framebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = m_Specification.size;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.17f, 0.17f, 0.17f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Set viewport and scissor
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_Specification.size.width);
        viewport.height = static_cast<float>(m_Specification.size.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_Specification.size};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VKFrameBuffer::EndRender()
    {
        auto commandBuffer = reinterpret_cast<VkCommandBuffer>(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());

        // Transition from COLOR_ATTACHMENT_OPTIMAL to SHADER_READ_ONLY_OPTIMAL
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = color_attachments[m_Specification.render_image_index].image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }
    void VKFrameBuffer::Resize(Extent2D size)
    {
        if (size.height == 0 || size.width == 0)
        {
            m_Specification.size = {1, 1};
        }

        // SetAccordingtoAspectRatio(m_Specification.size, size);
        float newAR = static_cast<float>(m_Specification.size.width) / static_cast<float>(m_Specification.size.height);
        float AR = static_cast<float>(size.width) / static_cast<float>(size.height);

        if (newAR > AR)
        {
            m_Specification.size.height = size.height;
            m_Specification.size.width = m_Specification.size.height * AR;
        }
        else
        {
            m_Specification.size.width = size.width;
            m_Specification.size.height = m_Specification.size.width / AR;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(VKDevice::Get().getPhysicalDevice(), &deviceProperties);
        uint32_t maxFramebufferWidth = deviceProperties.limits.maxFramebufferWidth;
        uint32_t maxFramebufferHeight = deviceProperties.limits.maxFramebufferHeight;

        // Adjust dimensions if they exceed limits
        m_Specification.size.width = std::min(size.width, maxFramebufferWidth);
        m_Specification.size.height = std::min(size.height, maxFramebufferHeight);

        VV_CORE_INFO("Extent Resized with WIDTH: {0}, HEIGHT {1} at {2} aspect ratio.", m_Specification.size.width, m_Specification.size.height, AR);

        vkDeviceWaitIdle(VKDevice::Get().device());
        clean();
        create_resources();
    }
    void *VKFrameBuffer::ReadPixel(uint32_t attachment_index, uint32_t x, uint32_t y)
    {
        VV_CORE_ASSERT(attachment_index < color_attachments.size());

        VkDevice device = VKDevice::Get().device();
        VkFormat format = getVKFormat(m_Specification.attachments[attachment_index]);

        // Determine pixel size based on format
        VkDeviceSize pixelSize = (format == VK_FORMAT_R32_SINT) ? sizeof(int32_t) : sizeof(uint32_t);
        VkDeviceSize bufferSize = pixelSize;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create staging buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VKDevice::Get().findMemoryType(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate staging buffer memory!");
        }
        vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

        VkCommandBuffer commandBuffer = VKDevice::Get().beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = color_attachments[attachment_index].image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        // Copy the pixel from the image to the staging buffer
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {static_cast<int32_t>(x), static_cast<int32_t>(y), 0};
        region.imageExtent = {1, 1, 1};

        vkCmdCopyImageToBuffer(
            commandBuffer,
            color_attachments[attachment_index].image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            stagingBuffer,
            1,
            &region);

        // Transition back to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VKDevice::Get().endSingleTimeCommands(commandBuffer);

        // Allocate memory for the result
        void *result = malloc(bufferSize);

        // Map memory and read data
        void *data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);

    
        memcpy(result, data, bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);
        // Cleanup
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        return result;
    }

    void VKFrameBuffer::create_resources()
    {

        if (m_Specification.seperate_renderpass)
        {
            CreateRenderpass();
        }
        int color_attachments_size;
        if (std::find(m_Specification.attachments.begin(), m_Specification.attachments.end(), FrameBufferFormat::Depth32) != m_Specification.attachments.end())
        {
            color_attachments_size = m_Specification.attachments.size() - 1;
        }
        else
        {
            color_attachments_size = m_Specification.attachments.size();
        }

        std::vector<VkImageView> framebufferAttachments;
        framebufferAttachments.reserve(m_Specification.attachments.size());
        //VV_CORE_TRACE("color attachments {0}", color_attachments_size);
        for (int i = 0; i < color_attachments_size; i++)
        {

            if (m_Specification.attachments[i] == FrameBufferFormat::None || m_Specification.attachments[i] == FrameBufferFormat::Depth32)
            {
                continue;
            }
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = getVKFormat(m_Specification.attachments[i]);
            imageInfo.extent.width = m_Specification.size.width;
            imageInfo.extent.height = m_Specification.size.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            if (m_Specification.attachments[i] == FrameBufferFormat::R32S)
            {
                imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            }
            else
            {
                imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            }
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VKFrameBufferImageAttachment attachment;

            vkCreateImage(VKDevice::Get().device(), &imageInfo, nullptr, &attachment.image);

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(VKDevice::Get().device(), attachment.image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = VKDevice::Get().findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            vkAllocateMemory(VKDevice::Get().device(), &allocInfo, nullptr, &attachment.imageMemory);
            vkBindImageMemory(VKDevice::Get().device(), attachment.image, attachment.imageMemory, 0);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = attachment.image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = getVKFormat(m_Specification.attachments[i]);
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(VKDevice::Get().device(), &viewInfo, nullptr, &attachment.imageView) != VK_SUCCESS)
            {
                VV_CORE_ERROR("ImageView Not Created for color attachments!");
            };
            color_attachments.push_back(attachment);
            framebufferAttachments.push_back(color_attachments[i].imageView);
        }

        if (std::find(m_Specification.attachments.begin(), m_Specification.attachments.end(), FrameBufferFormat::Depth32) != m_Specification.attachments.end())
        {
            VkImageCreateInfo depthImageInfo = {};
            depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
            depthImageInfo.format = getVKFormat(FrameBufferFormat::Depth32);
            depthImageInfo.extent.width = m_Specification.size.width;
            depthImageInfo.extent.height = m_Specification.size.height;
            depthImageInfo.extent.depth = 1;
            depthImageInfo.mipLevels = 1;
            depthImageInfo.arrayLayers = 1;
            depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VKFrameBufferImageAttachment depth_attachment;

            vkCreateImage(VKDevice::Get().device(), &depthImageInfo, nullptr, &depth_atachment.image);

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(VKDevice::Get().device(), depth_atachment.image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = VKDevice::Get().findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            vkAllocateMemory(VKDevice::Get().device(), &allocInfo, nullptr, &depth_atachment.imageMemory);
            vkBindImageMemory(VKDevice::Get().device(), depth_atachment.image, depth_atachment.imageMemory, 0);

            VkImageViewCreateInfo depthViewInfo{};
            depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            depthViewInfo.image = depth_atachment.image;
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            depthViewInfo.format = getVKFormat(FrameBufferFormat::Depth32);
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;

            vkCreateImageView(VKDevice::Get().device(), &depthViewInfo, nullptr, &depth_atachment.imageView);

            framebufferAttachments.push_back(depth_atachment.imageView);
            //VV_CORE_TRACE("framebuffer attachments {0}", framebufferAttachments.size());
        }

        // In create_resources, after creating the images:
        for (int i = 0; i < color_attachments.size(); i++)
        {
            VkCommandBuffer commandBuffer = VKDevice::Get().beginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = color_attachments[i].image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            vkCmdPipelineBarrier(
                commandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VKDevice::Get().endSingleTimeCommands(commandBuffer);
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        if (m_Specification.seperate_renderpass)
        {
            if (m_Renderpass == VK_NULL_HANDLE)
            {
                CreateRenderpass();
            }
            framebufferInfo.renderPass = m_Renderpass;
        }
        else
        {
            framebufferInfo.renderPass = *reinterpret_cast<VkRenderPass *>(RenderCommand::GetRendererAPI()->GetRenderpass());
        }
        framebufferInfo.attachmentCount = static_cast<uint32_t>(framebufferAttachments.size());
        framebufferInfo.pAttachments = framebufferAttachments.data();
        framebufferInfo.width = m_Specification.size.width;
        framebufferInfo.height = m_Specification.size.height;
        framebufferInfo.layers = 1;

        //VV_CORE_TRACE("framebuffer attachments {0}", framebufferAttachments.size());

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

        framebufferimageID = ImGui_ImplVulkan_AddTexture(sampler, color_attachments[m_Specification.render_image_index].imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    void VKFrameBuffer::clean()
    {
        vkDestroySampler(VKDevice::Get().device(), sampler, nullptr);
        vkDestroyFramebuffer(VKDevice::Get().device(), m_Framebuffer, nullptr);
        for (auto &attachment : color_attachments)
        {
            vkDestroyImageView(VKDevice::Get().device(), attachment.imageView, nullptr);
            vkDestroyImage(VKDevice::Get().device(), attachment.image, nullptr);
            vkFreeMemory(VKDevice::Get().device(), attachment.imageMemory, nullptr);
        }

        colorFormats.clear();
        color_attachments.clear();

        vkDestroyRenderPass(VKDevice::Get().device(), m_Renderpass, nullptr);
    }
    void VKFrameBuffer::CreateRenderpass()
    {

        std::vector<VkAttachmentDescription> attachments;
        for (auto &vk_format : m_Specification.attachments)
        {
            if (vk_format != FrameBufferFormat::None && vk_format != FrameBufferFormat::Depth32)
            {
                colorFormats.push_back(getVKFormat(vk_format));
            }
            else if (vk_format == FrameBufferFormat::Depth32)
            {
                depthFormat = getVKFormat(vk_format);
            }
        }

        for (const auto &format : colorFormats)
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = format;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(colorAttachment);
        }
        if (depthFormat)
        {
            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = depthFormat;
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(depthAttachment);
        }
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        for (size_t i = 0; i < colorFormats.size(); ++i)
        {
            VkAttachmentReference colorRef{};
            colorRef.attachment = static_cast<uint32_t>(i);
            colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentRefs.push_back(colorRef);
        }
        VV_CORE_INFO("Created {} color attachments.", colorAttachmentRefs.size());

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = static_cast<uint32_t>(colorFormats.size());
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
        subpass.pColorAttachments = colorAttachmentRefs.data();
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(VKDevice::Get().device(), &renderPassInfo, nullptr, &m_Renderpass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VKFrameBuffer::BeginRenderpass()
    {

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_Renderpass;
        renderPassInfo.framebuffer = m_Framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_Specification.size;

        // TODO:make this set o framebuffer creation
        std::vector<VkClearValue> clearValues{3};
        clearValues[0].color = {{0.17f, 0.17f, 0.17f, 1.0f}};
        clearValues[1].color.int32[0] = -1;
        clearValues[2].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VKFrameBuffer::EndRenderpass()
    {
        vkCmdEndRenderPass(RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer());
    }
}