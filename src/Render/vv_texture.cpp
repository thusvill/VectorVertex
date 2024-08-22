#include "vv_texture.hpp"
#include "vv_buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <Log.h>

namespace VectorVertex
{
    VVTexture::VVTexture(VVDevice &device) : device(device), textureImage(nullptr), textureImageView(VK_NULL_HANDLE), textureSampler(VK_NULL_HANDLE)
    {
    }
    VVTexture::~VVTexture()
    {
        if (textureSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(device.device(), textureSampler, nullptr);
        }
        if (textureImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device.device(), textureImageView, nullptr);
        }
        delete textureImage;
    }
    void VVTexture::createTextureImage(const std::string &filePath)
    {
        stbi_uc *pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4; // Assuming STBI_rgb_alpha means 4 bytes per pixel

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        stagingBuffer = createStagingBuffer(pixels, imageSize);

        textureImage = new VVImage(device);
        
        textureImage->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, texWidth, texHeight);
        transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        destroyStagingBuffer(stagingBuffer, stagingBufferMemory);
        stbi_image_free(pixels);
        valid = true;
    }
    void VVTexture::createTextureImageView()
    {
        textureImageView = textureImage->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    void VVTexture::createTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    void VVTexture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        textureImage->transitionImageLayout(device.getCommandPool(), device.graphicsQueue(), VK_FORMAT_R8G8B8A8_SRGB, oldLayout, newLayout);
    }
    void VVTexture::copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height)
    {
        VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, textureImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        device.endSingleTimeCommands(commandBuffer);
    }
    VkBuffer VVTexture::createStagingBuffer(const void *data, VkDeviceSize size)
    {
        

        device.createBuffer(size,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory);

        void *mappedData;
        vkMapMemory(device.device(), stagingBufferMemory, 0, size, 0, &mappedData);
        memcpy(mappedData, data, (size_t)size);
        vkUnmapMemory(device.device(), stagingBufferMemory);

        return stagingBuffer;
    }
    void VVTexture::destroyStagingBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory)
    {
        vkDestroyBuffer(device.device(), buffer, nullptr);
        vkFreeMemory(device.device(), bufferMemory, nullptr);
    }
}