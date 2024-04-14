#include "vv_framebuffer.hpp"

namespace VectorVertex{
    VVFramebuffer::VVFramebuffer(VVDevice device) 
    {
        m_device = device.device();
        vv_device = &device;
    
    }

    void VVFramebuffer::Begin(VkCommandPool commandPool, uint32_t width, uint32_t height)
    {
        VkFormat offscreenFormat = VK_FORMAT_R8G8B8A8_UNORM; // Example format, adjust as needed
        VkRenderPass offscreenRenderPass = CreateOffscreenRenderPass(m_device, offscreenFormat);

        // Offscreen image and view creation
        // Create offscreen image
        VkImageCreateInfo imageCreateInfo = {};
        // Fill image create info
        VkImage offscreenImage;
        vkCreateImage(m_device, &imageCreateInfo, nullptr, &offscreenImage);

        // Create image view
        VkImageViewCreateInfo imageViewCreateInfo = {};
        // Fill image view create info
        VkImageView offscreenImageView;
        vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &offscreenImageView);

        // Offscreen framebuffer creation
        VkFramebuffer offscreenFramebuffer = CreateOffscreenFramebuffer(m_device, offscreenRenderPass, offscreenImageView, width, height);

        // Offscreen command buffer recording
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        
        vkAllocateCommandBuffers(m_device, &allocInfo, &offscreenCommandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        vkBeginCommandBuffer(offscreenCommandBuffer, &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = offscreenRenderPass;
        renderPassInfo.framebuffer = offscreenFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.width = width;
        renderPassInfo.renderArea.extent.height = height;

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(offscreenCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VVFramebuffer::End(VkQueue graphics_queue)
    {
        vkCmdEndRenderPass(offscreenCommandBuffer);

        vkEndCommandBuffer(offscreenCommandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &offscreenCommandBuffer;

        vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);

        
    }

    VkImage VVFramebuffer::RenderFramebufferToImage(VkCommandPool commandPool, uint32_t width, uint32_t height)
    {
        // Create the image
        VkImageCreateInfo imageCreateInfo = {};
        // Fill in image create info
        VkImage renderedImage;
        vkCreateImage(m_device, &imageCreateInfo, nullptr, &renderedImage);

        // Allocate memory for the image
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_device, renderedImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // Implement findMemoryType according to your memory requirements

        VkDeviceMemory imageMemory;
        vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory);

        // Bind image memory
        vkBindImageMemory(m_device, renderedImage, imageMemory, 0);

          // Return the Vulkan image
        return renderedImage;
    }

VkRenderPass VVFramebuffer::CreateOffscreenRenderPass(VkDevice device, VkFormat format)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass!");
    }

    return renderPass;
}
VkFramebuffer VVFramebuffer::CreateOffscreenFramebuffer(VkDevice device, VkRenderPass renderPass, VkImageView offscreenImageView, uint32_t width, uint32_t height)
{
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &offscreenImageView;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create offscreen framebuffer!");
    }

    return framebuffer;
}

uint32_t VVFramebuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vv_device->getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
}