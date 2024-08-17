#pragma once
#include <VectorVertex.hpp>

namespace VectorVertex
{
    class VVOffscreen
    {
    public:
        VVOffscreen() = default;
        VVOffscreen(VVDevice *vv_device, VkExtent2D extent, VkRenderPass renderPass);
        ~VVOffscreen();

        void Initialize(VkDescriptorPool descriptorPool);
        void StartFrame(VkCommandBuffer commandBuffer);
        void StopFrame(VkCommandBuffer commandBuffer);
        void SetupImGuiTexture(VkDescriptorSet imguiDescriptorSet);
        VkDescriptorSet GetImGuiDescriptorSet() const { return imguiDescriptorSet; }
        VkDescriptorImageInfo imageInfoForImGui{};

    private:
        VVDevice *vvDevice;
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkExtent2D extent;

        VkRenderPass offscreenRenderPass;
        VkFramebuffer offscreenFramebuffer;
        VkImage offscreenImage;
        VkImageView offscreenImageView;
        VkDeviceMemory offscreenImageMemory;
        VkImage depthImage;
        VkImageView depthImageView;
        VkDeviceMemory depthImageMemory;
        VkSampler offscreenSampler;
        VkDescriptorSet imguiDescriptorSet;

        void CreateOffscreenResources();
        VkFormat findDepthFormat_offscreen(VVDevice *device);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

}