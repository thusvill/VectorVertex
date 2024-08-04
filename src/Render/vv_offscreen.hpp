#pragma once
#include <VectorVertex.hpp>
#include "vv_renderer.hpp"
#include "vv_framebuffer.hpp"


namespace VectorVertex
{
    class VVOffscreen{
    public:
        VVOffscreen() = default;
        VVOffscreen(VVDevice& vv_device, VkExtent2D extent);
        VkCommandBuffer BeginFrame();
       // void BeginRenderPass();
        void EndFrame();
        VkDescriptorSet GetImageDescriptorSet() { return descriptorSet; }
        int GetFrameIndex() const {
            assert(is_frame_started && "Cannot get frame index when frame not in progress");
            return frame_index;
        }
        VkCommandBuffer GetCommandBuffer() { return command_buffer; }
        const VkDescriptorPool& GetDescriptorPool() const { return descriptorPool; }

        VkRenderPass GetRenderpass() const { return offscreenRenderPass;}
        VkDescriptorSetLayout GetDescriptorsetlayout() const {return descriptorSetLayout;}

    private:
        void SetupImages(VVDevice* vv_device);
        void createOffscreenRenderPass(VkFormat colorFormat, VkRenderPass& offscreenRenderPass);
        void transitionImageLayout(VkCommandPool commandPool, VkQueue graphicsQueue,VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        VVFramebuffer framebuffer;

        int frame_index{0};
        bool is_frame_started = false;
        VkRenderPass offscreenRenderPass;
        // VkImage offscreenImage;
        // VkDeviceMemory offscreenImageMemory;
        // VkImageView offscreenImageView;
        // VkFormat offscreenImageFormat;
        VkCommandBuffer command_buffer;
        VkExtent2D window_size;
        VVDevice& vv_device;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;
        VkSampler textureSampler;
        VkPipelineLayout pipelinelayout;

    };
}