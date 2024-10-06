#pragma once
#include <vulkan/vulkan.h>
#include "vk_device.hpp"
#include "Log.h"

namespace VectorVertex
{
    class VKImage
    {
    public:
        VKImage();
        ~VKImage();
        void createImage(uint32_t width, uint32_t height, VkFormat format,
                         VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties);

        VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

        void transitionImageLayout(VkCommandPool commandPool, VkQueue graphicsQueue,
                                   VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        VkImage getImage() const { return image; }
        VkDeviceMemory getImageMemory() const { return imageMemory; }

    private:;
        
        VkImage image;
        VkDeviceMemory imageMemory;

        void allocateMemory(VkMemoryPropertyFlags properties, VkMemoryRequirements memRequirements);
    };
}