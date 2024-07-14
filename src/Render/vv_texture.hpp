#pragma once
#include "vv_device.hpp"
#include <string.h>
#include <unordered_map>


namespace VectorVertex
{
    class VVTexture{
        public:
            VVTexture(VVDevice &device, const std::string &filepath);
            // VVTexture(const VVTexture&) = delete;
            // VVTexture& operator=(const VVTexture&) = delete;
            // VVTexture(VVTexture&&) = delete;
            // VVTexture& operator=(VVTexture&&) = delete;

            ~VVTexture();

            VkSampler getSampler() const { return sampler; }
            VkImageView getImageView() const { return imageView; }
            VkImageLayout getImageLayout() const { return imageLayout; }

            private:
            void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
                VVDevice& vvDevice;
                VkImage image;
                VkDeviceMemory imageMemory;
                VkImageView imageView;
                VkSampler sampler;
                VkImageLayout imageLayout;
                VkFormat imageFormat;
    };
    struct TextureData{
        TextureData()=default;
        uint32_t ID;
        VVTexture* texture;
        VkDescriptorImageInfo vk_image_info{};

    };

    class VVTextureLibrary{
        public:
            static std::unordered_map<std::string, TextureData> m_Textures;
            static uint32_t LoadTexture(VVDevice* device, std::string name, std::string path);
            static TextureData getTexture(uint32_t id);
            static TextureData getTexture(std::string name);

    };

} // namespace VectorVertex
