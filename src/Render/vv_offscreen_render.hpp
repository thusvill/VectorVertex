#pragma once
#include <vulkan/vulkan.h>
#include <Log.h>
#include <vv_device.hpp>
#include <vv_frame_info.hpp>
#include <vv_swap_chain.hpp>

namespace VectorVertex
{
    class OffscreenRender
    {
    public:
        OffscreenRender(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent, VkCommandPool commandPool, VkQueue graphicsQueue);
        ~OffscreenRender();

        void startFrame();
        void endFrame();
        int GetFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }
        VkCommandBuffer getCommandBuffer() const { return commandBuffer; }
        VkDescriptorSet getImGuiDescriptorSet() const;

        void ResizeCallback(VkExtent2D new_extent);

    private:
        void createOffscreenResources();
        void createFramebuffer();
        void createRenderPass();
        void createCommandBuffer();
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSet();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkExtent2D extent;
        VkCommandPool commandPool;
        VkQueue graphicsQueue;

        VkImage offscreenImage;
        VkDeviceMemory offscreenImageMemory;
        VkImageView offscreenImageView;
        VkFramebuffer offscreenFramebuffer;
        VkRenderPass offscreenRenderPass;
        VkCommandBuffer commandBuffer;

        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;
        VkSampler offscreenSampler;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };

}