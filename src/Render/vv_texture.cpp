#include "vv_texture.hpp"
#include "vv_buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <Log.h>

namespace VectorVertex
{
    std::unordered_map<uint64_t, Ref<VVTexture>> VVTextureLibrary::m_Textures;
    uint64_t VVTextureLibrary::default_uuid;
    Scope<VVDescriptorPool> VVTextureLibrary::texture_pool;
    Scope<VVDescriptorSetLayout> VVTextureLibrary::textureImageDescriptorLayout;

    VVTexture::VVTexture(VVDevice &device, const std::string &path) : device(device)
    {
        createTextureImage(path);
        createTextureImageView();
        createTextureSampler();
    }
    VVTexture::~VVTexture()
    {
        if (data.m_textureSampler != VK_NULL_HANDLE && device.device() != VK_NULL_HANDLE)
        {
            vkDestroySampler(device.device(), data.m_textureSampler, nullptr);
        }
        if (data.m_textureImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device.device(), data.m_textureImageView, nullptr);
        }
        delete data.m_textureImage;
    }
    void VVTexture::createTextureImage(const std::string &filePath)
    {
        stbi_uc *pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4; // Assuming STBI_rgb_alpha means 4 bytes per pixel

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        m_stagingBuffer = createStagingBuffer(pixels, imageSize);

        data.m_textureImage = new VVImage(device);
        data.m_path = filePath;

        data.m_textureImage->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(m_stagingBuffer, texWidth, texHeight);
        transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        destroyStagingBuffer(m_stagingBuffer, m_stagingBufferMemory);
        stbi_image_free(pixels);
        valid = true;
    }
    void VVTexture::createTextureImageView()
    {
        data.m_textureImageView = data.m_textureImage->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
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

        if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &data.m_textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    void VVTexture::loadTexture(const std::string &newPath)
    {
        createTextureImage(newPath);
        createTextureImageView();
        createTextureSampler();
    }
    void VVTexture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        data.m_textureImage->transitionImageLayout(device.getCommandPool(), device.graphicsQueue(), VK_FORMAT_R8G8B8A8_SRGB, oldLayout, newLayout);
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

        vkCmdCopyBufferToImage(commandBuffer, buffer, data.m_textureImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        device.endSingleTimeCommands(commandBuffer);
    }
    VkBuffer VVTexture::createStagingBuffer(const void *data, VkDeviceSize size)
    {

        device.createBuffer(size,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            m_stagingBuffer, m_stagingBufferMemory);

        void *mappedData;
        vkMapMemory(device.device(), m_stagingBufferMemory, 0, size, 0, &mappedData);
        memcpy(mappedData, data, (size_t)size);
        vkUnmapMemory(device.device(), m_stagingBufferMemory);

        return m_stagingBuffer;
    }
    void VVTexture::destroyStagingBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory)
    {
        vkDestroyBuffer(device.device(), buffer, nullptr);
        vkFreeMemory(device.device(), bufferMemory, nullptr);
    }

    void VVTextureLibrary::InitTextureLib(VVDevice &device)
    {
        default_uuid = Create(device, "default", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Textures/prototype_512x512_grey2.png");
        texture_pool = VVDescriptorPool::Builder(device)
                           .setMaxSets(VVSwapChain::MAX_FRAMES_IN_FLIGHT * 2)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VVSwapChain::MAX_FRAMES_IN_FLIGHT)
                           .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                           .build();

        textureImageDescriptorLayout = VVDescriptorSetLayout::Builder(device)
                                           .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                                           .build();
        VV_CORE_INFO("Initilized Texture Library!");
    }

    uint64_t VVTextureLibrary::Create(VVDevice &device,std::string name, std::string path)
    {

        Ref<VVTexture> texture = CreateRef<VVTexture>(device, path);
        texture->data.m_Name = name;
        VV_CORE_INFO("Created Texture from:{0} as:{1} with UUID:{2}", path, name, texture->data.m_ID);
        m_Textures[texture->data.m_ID] = texture;
        return texture->data.m_ID;
    }
    void VVTextureLibrary::CreateWithUUID(VVDevice &device,UUID id, std::string name, std::string path)
    {
        Ref<VVTexture> texture = CreateRef<VVTexture>(device, path);
        texture->data.m_Name = name;
        texture->data.m_ID = id;
        VV_CORE_INFO("Created Texture from:{0} as:{1} with UUID:{2}", path, name, texture->data.m_ID);
        m_Textures[texture->data.m_ID] = texture;
    }
    void VVTextureLibrary::AddTexture(Ref<VVTexture> texture)
    {
        if (!texture)
        {
            VV_CORE_ERROR("Texture Is Null or No initialized!");
            return;
        }
        m_Textures[texture->data.m_ID] = texture;
    }
    VVTexture &VVTextureLibrary::GetTexture(UUID ID)
    {
        if (!m_Textures[ID])
        {
            VV_CORE_ERROR("No Texture assigned with UUID: {}", ID);
        }
        return *m_Textures.at(ID);
    }
    void VVTextureLibrary::DeleteTexture(UUID ID)
    {
        if (ID != default_uuid)
        {
            m_Textures.erase(ID);
        }
    }
    void VVTextureLibrary::UpdateDescriptors()
    {
        if (!VVSwapChain::isWaitingForFence)
        {
            VV_CORE_TRACE("Fence Done!");
            for (auto &kv : m_Textures)
            {
                auto imageInfo = VVTextureLibrary::GetTexture(kv.first).getDescriptorImageInfo();
                VVDescriptorWriter(*VVTextureLibrary::textureImageDescriptorLayout, *VVTextureLibrary::texture_pool)
                    .writeImage(0, &imageInfo)
                    .build(VVTextureLibrary::GetTexture(kv.first).data.m_descriptorSet);
            }
        }
        else
        {
            VV_CORE_TRACE("Waiting For Fence");
        }
    }
    void VVTextureLibrary::ClearLibrary()
    {
        m_Textures.clear();
        VV_CORE_INFO("Texture Library cleared!");
    }
}