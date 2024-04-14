#pragma once
#include "vv_device.hpp"
#include <string.h>


namespace VectorVertex
{
    class VVTexture{
        public:
            VVTexture(VVDevice &device, const std::string &filepath);
            VVTexture(const VVTexture&) = delete;
            VVTexture& operator=(const VVTexture&) = delete;
            VVTexture(VVTexture&&) = delete;
            VVTexture& operator=(VVTexture&&) = delete;

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

} // namespace VectorVertex
