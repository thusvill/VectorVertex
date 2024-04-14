#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <Log.h>
#include "vv_device.hpp"
#include <imgui.h>

namespace VectorVertex
{

    class VVFramebuffer
    {
        public:
        VVFramebuffer(VVDevice* device, VkRenderPass renderpass, uint32_t width, uint32_t height);
        VkFramebuffer GetFrameBuffer() { return framebuffer; }
        private:
            VkFramebuffer framebuffer;
            VVDevice *vvDevice;
            VkImage CreateImage(VkDevice device,VkImageUsageFlags usage, uint32_t WIDTH, uint32_t HEIGHT);
            // Example function to create a color attachment image view
            VkImageView createColorAttachmentImageView(VkDevice device, VkImage image, VkFormat format)
            {
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = image;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = format;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;

                VkImageView imageView;
                if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
                {
                    // Handle image view creation failure
                }

                return imageView;
            }

            // Example function to create a depth attachment image view
            VkImageView createDepthAttachmentImageView(VkDevice device, VkImage image, VkFormat format)
            {
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = image;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = format;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;

                VkImageView imageView;
                if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
                {
                    // Handle image view creation failure
                }

                return imageView;
            }

            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
            {
                VkPhysicalDeviceMemoryProperties memProperties;
                vkGetPhysicalDeviceMemoryProperties(vvDevice->getPhysicalDevice(), &memProperties);

                for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
                {
                    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                    {
                        return i;
                    }
                }

                throw std::runtime_error("failed to find suitable memory type");
            }
    };
}