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
        OffscreenRender(VVDevice *vv_device, VkExtent2D extent, VkCommandPool commandPool, VkQueue graphicsQueue);
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

        void ResizeCallback(VkExtent2D newExtent, VVDevice* vv_device);

        VkRenderPass getRenderPass() {return offscreenRenderPass;}
        VkDescriptorSetLayout getDescriptorLayout() {return descriptorSetLayout;}

    private:
        void createOffscreenResources(VVDevice* vv_device);
        void createFramebuffer(VVDevice *vv_device);
        void createRenderPass(VVDevice* vv_device);
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
        VkImage offscreenDepthImage;
        VkDeviceMemory offscreenImageMemory;
        VkDeviceMemory offscreenDepthImageMemory;
        VkImageView offscreenImageView;
        VkImageView offscreenDepthImageView;
        VkFramebuffer offscreenFramebuffer;
        VkRenderPass offscreenRenderPass;
        VkCommandBuffer commandBuffer;

        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;
        VkSampler offscreenSampler;

        uint32_t currentImageIndex;

        VkFormat findDepthFormat_offscreen(VVDevice* device);
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };

}