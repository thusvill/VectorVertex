#pragma once

#include "vk_window.hpp"
<<<<<<<< HEAD:VectorVertex/src/Plattform/Vulkan/vk_device.hpp
#include <Core/Log.h>
========
#include <Log.h>
>>>>>>>> 400cd19c24c23a0b77a9a6741370ff57828b1ef2:src/Plattform/Vulkan/vk_device.hpp
#include <vvpch.hpp>

namespace VectorVertex
{

  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct QueueFamilyIndices
  {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
  };

  class VKDevice
  {
  public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
<<<<<<<< HEAD:VectorVertex/src/Plattform/Vulkan/vk_device.hpp
    VKDevice() = default;
    VKDevice(GLFWwindow *window);
========
VKDevice() = default;
    VKDevice(GLFWwindow* window);
>>>>>>>> 400cd19c24c23a0b77a9a6741370ff57828b1ef2:src/Plattform/Vulkan/vk_device.hpp
    ~VKDevice();

    // Not copyable or movable
    VKDevice(const VKDevice &) = delete;
    VKDevice &operator=(const VKDevice &) = delete;
    VKDevice(VKDevice &&) = delete;
    VKDevice &operator=(VKDevice &&) = delete;

    VkCommandPool getCommandPool() { return commandPool; }
    VkDevice device() { return device_; }
    VkSurfaceKHR surface() { return surface_; }
    VkQueue graphicsQueue() { return graphicsQueue_; }
    VkQueue presentQueue() { return presentQueue_; }
    VkInstance getInstance() const { return instance_; }
    VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }

    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
    VkFormat findSupportedFormat(
        const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    // Buffer Helper Functions
    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(
        VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

    void createImageWithInfo(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);

    VkPhysicalDeviceProperties properties;
    VkInstance instance_;

<<<<<<<< HEAD:VectorVertex/src/Plattform/Vulkan/vk_device.hpp
    static VKDevice &Get()
    {
========
    static VKDevice& Get() {
>>>>>>>> 400cd19c24c23a0b77a9a6741370ff57828b1ef2:src/Plattform/Vulkan/vk_device.hpp
      VV_CORE_ASSERT(s_Device, "Device is NULL!");
      return *s_Device;
    }

  private:
<<<<<<<< HEAD:VectorVertex/src/Plattform/Vulkan/vk_device.hpp
    static VKDevice *s_Device;
========
    static VKDevice* s_Device;
>>>>>>>> 400cd19c24c23a0b77a9a6741370ff57828b1ef2:src/Plattform/Vulkan/vk_device.hpp
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void hasGflwRequiredInstanceExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    GLFWwindow *window;
    VkCommandPool commandPool;

    VkDevice device_;
    VkSurfaceKHR surface_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME};
<<<<<<<< HEAD:VectorVertex/src/Plattform/Vulkan/vk_device.hpp
  };

========

  };


>>>>>>>> 400cd19c24c23a0b77a9a6741370ff57828b1ef2:src/Plattform/Vulkan/vk_device.hpp
} // namespace VectorVertex