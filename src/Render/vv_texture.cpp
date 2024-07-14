#include "vv_texture.hpp"
#include "vv_buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <Log.h>


namespace VectorVertex{

VVTexture::VVTexture(VVDevice &device, const std::string &filepath) :vvDevice{device}
{
    int width, height, channels, bytesPerPixel;

    stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, 4);

    VVBuffer stagingBuffer{
        vvDevice, 4, static_cast<uint32_t>(width * height * 4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer(data);

    imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = imageFormat;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    vvDevice.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vvDevice.copyBufferToImage(stagingBuffer.getBuffer(), image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1);

    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    vkCreateSampler(vvDevice.device(), &samplerInfo, nullptr, &sampler);

    VkImageViewCreateInfo imageViewInfo{};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = imageFormat;
    imageViewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.image = image;

    vkCreateImageView(vvDevice.device(), &imageViewInfo, nullptr, &imageView);

    stbi_image_free(data);
}
VVTexture::~VVTexture()
{
    vkDestroyImage(vvDevice.device(), image, nullptr);
    vkFreeMemory(vvDevice.device(), imageMemory, nullptr);
    vkDestroyImageView(vvDevice.device(), imageView, nullptr);
    vkDestroySampler(vvDevice.device(), sampler, nullptr);
}
void VVTexture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = vvDevice.beginSingleTimeCommands();
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        VV_CORE_ERROR("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    vvDevice.endSingleTimeCommands(commandBuffer);
}

std::unordered_map<std::string, VectorVertex::TextureData> VectorVertex::VVTextureLibrary::m_Textures;

uint32_t VVTextureLibrary::LoadTexture(VVDevice* device,std::string name, std::string path)
{
    if(name.empty() || path.empty()){
        VV_CORE_ERROR("Texture name or path is empty!");
        return -1;
    }
    if(m_Textures.find(name) != m_Textures.end()){
        VV_CORE_WARN("Texture already exists {}", name);
        return getTexture(name).ID;
    }else{

    auto tex = TextureData{};
    tex.ID = m_Textures.size();
    VVTexture data(*device, path);
    tex.texture = &data;
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = data.getImageLayout();
    imageInfo.imageView = data.getImageView();
    imageInfo.sampler = data.getSampler();
    tex.vk_image_info = imageInfo;

    m_Textures[name] = tex;
    VV_CORE_INFO("Created Texture: {} ID: {}", name, tex.ID);
    return tex.ID;
    }
}
TextureData VVTextureLibrary::getTexture(uint32_t id)
{
    for(const auto &pair: m_Textures){
        if(pair.second.ID == id){
            return pair.second;
        }
    }

    VV_CORE_ERROR("Texture not found: {}", id);
    return TextureData();
}
TextureData VVTextureLibrary::getTexture(std::string name)
{
    return m_Textures[name];
}
}