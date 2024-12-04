#pragma once

#include "vk_device.hpp"
#include <vvpch.hpp>

namespace VectorVertex
{

    class VKDescriptorSetLayout
    {
    public:
        class Builder
        {
        public:
            Builder(VKDevice &vkDevice) : vkDevice{vkDevice} {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<VKDescriptorSetLayout> build() const;

        private:
            VKDevice &vkDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        VKDescriptorSetLayout(
            VKDevice &vkDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~VKDescriptorSetLayout();
        VKDescriptorSetLayout(const VKDescriptorSetLayout &) = delete;
        VKDescriptorSetLayout &operator=(const VKDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout()  { return descriptorSetLayout; }

    private:
        VKDevice &vkDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class VKDescriptorWriter;
    };

    class VKDescriptorPool
    {
    public:
        class Builder
        {
        public:
            Builder(VKDevice &vkDevice) : vkDevice{vkDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<VKDescriptorPool> build() const;

        private:
            VKDevice &vkDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        VKDescriptorPool(
            VKDevice &VKDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~VKDescriptorPool();
        VKDescriptorPool(const VKDescriptorPool &) = delete;
        VKDescriptorPool &operator=(const VKDescriptorPool &) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

        VkDescriptorPool getPool() {return descriptorPool;}

        uint32_t maxSets;

    private:
        VKDevice &vkDevice;
        VkDescriptorPool descriptorPool;

        friend class VKDescriptorWriter;
    };

    class VKDescriptorWriter
    {
    public:
        VKDescriptorWriter(VKDescriptorSetLayout &setLayout, VKDescriptorPool &pool);

        VKDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        VKDescriptorWriter &writeBuffer(uint32_t binding,VkDescriptorSet descriptorSet ,VkDescriptorBufferInfo *bufferInfo);
        VKDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
        VKDescriptorWriter &writeImage(uint32_t binding, VkDescriptorSet descriptorSet, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        VKDescriptorSetLayout &setLayout;
        VKDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace VectorVertex