#pragma once
#include <vvpch.hpp>
#include "vk_swap_chain.hpp"
#include <Core/Log.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>

// TODO: Make this as Frame buffer and remove old framebuffer code

namespace VectorVertex
{
    class VKOffscreen
    {
    public:
        VKOffscreen() = default;
        VKOffscreen(VkExtent2D size);
        ~VKOffscreen();
        void StartRenderpass(VkCommandBuffer commandBuffer);
        void EndRendrepass(VkCommandBuffer commandBuffer);
        void Resize(VkExtent2D new_extent);
        VkExtent2D getViewSize() { return ViewExtent; }
        ImTextureID getFramebufferImage() { return imguiTextureId; }
        void SetAccordingtoAspectRatio(VkExtent2D &old_extent, VkExtent2D new_extent);

    private:
        void create_resources();
        void clean();
        VkExtent2D ViewExtent{800, 800};
        ImTextureID imguiTextureId;
        VkImage offscreenImage;
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
        VkDeviceMemory offscreenImageMemory;
        VkImageView offscreenImageView;
        VkFramebuffer offscreenFramebuffer;
        VkSampler sampler;
        VkDescriptorSet descriptorSet;
    };
} // namespace VectorVertex
