#include "vv_framebuffer.hpp"
namespace VectorVertex
{
VVFramebuffer::VVFramebuffer(VVDevice* device, VkRenderPass& renderpass, VkFormat colorFormat, uint32_t width, uint32_t height) : vvDevice(device)
{
    colorAttachmentView = createColorAttachmentImageView(vvDevice->device(), CreateImage(vvDevice->device(),VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, width, height), colorFormat);
    depthAttachmentView = createDepthAttachmentImageView(vvDevice->device(), CreateImage(vvDevice->device(),vvDevice->findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, width, height));
    std::vector<VkImageView> attachments= {colorAttachmentView, depthAttachmentView};

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderpass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    
    VkResult result = vkCreateFramebuffer(vvDevice->device(), &framebufferInfo, nullptr, &framebuffer);
    if (result != VK_SUCCESS)
    {
        VV_CORE_ERROR("Failed to create framebuffer");
    }
}

    VkImage VVFramebuffer::CreateImage(VkDevice device,VkFormat colorFormat,VkImageUsageFlags usage, uint32_t WIDTH, uint32_t HEIGHT)
    {
        VkImage image;
        VkDeviceMemory imageMemory;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = WIDTH;
        imageInfo.extent.height = HEIGHT;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = colorFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to create image");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to allocate image memory");
        }

        vkBindImageMemory(device, image, imageMemory, 0);

        return image;
    }

} // namespace VectorVertex