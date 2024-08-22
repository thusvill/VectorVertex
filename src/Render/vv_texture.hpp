#pragma once
#include "vv_device.hpp"
#include "vv_image.hpp"
#include <string.h>
#include <unordered_map>

namespace VectorVertex
{
    class VVTexture
    {
    public:
        VVTexture() = default;
        VVTexture(VVDevice &device);
        ~VVTexture();

        VVTexture(const VVTexture &) = delete;
        VVTexture &operator=(const VVTexture &) = delete;

        void createTextureImage(const std::string &filePath);
        void createTextureImageView();
        void createTextureSampler();

        VkImageView getImageView() const
        {

            return textureImageView;
        }
        VkSampler getSampler() const
        {

            return textureSampler;
        }
        VkDescriptorImageInfo getDescriptorImageInfo()
        {
            if (textureImageView == VK_NULL_HANDLE)
            {
                createTextureImageView();
            }
            if (textureSampler == VK_NULL_HANDLE)
            {
                createTextureSampler();
            }

            VkDescriptorImageInfo info{};
            info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            info.imageView = getImageView();
            info.sampler = getSampler();
            return info;
        }

        explicit operator bool() const
        {
            return valid;
        }

    private:
        bool valid = false;
        VVDevice &device;

        VVImage *textureImage;
        VkImageView textureImageView;
        VkSampler textureSampler;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
        VkBuffer createStagingBuffer(const void *data, VkDeviceSize size);
        void destroyStagingBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);

        int texWidth, texHeight, texChannels;
    };

} // namespace VectorVertex
