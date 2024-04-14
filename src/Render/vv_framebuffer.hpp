#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <cstring>
#include "vv_device.hpp"
namespace VectorVertex{ 
class VVFramebuffer
{
    public:
    VVFramebuffer(VVDevice device);
    VVFramebuffer() = default;
    void Begin(VkCommandPool commandPool, uint32_t width, uint32_t height);
    void End(VkQueue graphics_queue);
    VkImage RenderFramebufferToImage(VkCommandPool commandPool, uint32_t width, uint32_t height);

private:
    VkCommandBuffer offscreenCommandBuffer;
    VkDevice m_device;
    VVDevice* vv_device;
    VkImage renderedImage;
    VkDeviceSize imageSize;

    VkRenderPass CreateOffscreenRenderPass(VkDevice device, VkFormat format);
    VkFramebuffer CreateOffscreenFramebuffer(VkDevice device, VkRenderPass renderPass, VkImageView offscreenImageView, uint32_t width, uint32_t height);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
}