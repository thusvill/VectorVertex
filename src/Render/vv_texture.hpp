#pragma once
#include "vv_device.hpp"
#include "vv_image.hpp"
#include "vv_descriptors.hpp"
#include "vv_swap_chain.hpp"
#include <vv_uuid.hpp>
#include <string.h>
#include <unordered_map>

namespace VectorVertex
{
    struct TextureData
    {
        UUID m_ID;
        std::string m_Name;
        VVImage *m_textureImage;
        VkImageView m_textureImageView;
        VkSampler m_textureSampler;
        VkDescriptorSet m_descriptorSet;
    };
    class VVTexture
    {
    public:
        VVTexture(VVDevice &device, const std::string &path);
        VVTexture(const VVTexture &) = delete;
        VVTexture &operator=(const VVTexture &) = delete;
        VVTexture(VVTexture &&) noexcept = default;
        VVTexture &operator=(VVTexture &&) noexcept = default;
        ~VVTexture();

        void createTextureImage(const std::string &filePath);
        void createTextureImageView();
        void createTextureSampler();

        VkImageView getImageView() const
        {

            return data.m_textureImageView;
        }
        VkSampler getSampler() const
        {

            return data.m_textureSampler;
        }
        VkDescriptorImageInfo getDescriptorImageInfo()
        {
            if (data.m_textureImageView == VK_NULL_HANDLE)
            {
                createTextureImageView();
            }
            if (data.m_textureSampler == VK_NULL_HANDLE)
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

        TextureData data;

    private:
        bool valid = false;
        VVDevice &device;

        VkBuffer m_stagingBuffer;
        VkDeviceMemory m_stagingBufferMemory;

        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
        VkBuffer createStagingBuffer(const void *data, VkDeviceSize size);
        void destroyStagingBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);

        int texWidth, texHeight, texChannels;
    };

    class VVTextureLibrary
    {
    public:
        // VVTextureLibrary();
        //~VVTextureLibrary();
        static void InitTextureLib(VVDevice &device);
        static uint64_t Create(VVDevice &device, std::string, std::string path);
        static void AddTexture(Ref<VVTexture> texture);
        static VVTexture& GetTexture(UUID ID);
        static void ClearLibrary();

        static std::unordered_map<uint64_t, Ref<VVTexture>> m_Textures;
        static uint64_t default_uuid;
        static Scope<VVDescriptorPool> texture_pool;
        static Scope<VVDescriptorSetLayout> textureImageDescriptorLayout;
    };
} // namespace VectorVertex
