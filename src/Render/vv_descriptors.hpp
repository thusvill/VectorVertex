#pragma once

#include "vv_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace VectorVertex
{

    class LveDescriptorSetLayout
    {
    public:
        class Builder
        {
        public:
            Builder(VVDevice &vvDevice) : vvDevice{vvDevice} {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<LveDescriptorSetLayout> build() const;

        private:
            VVDevice &vvDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        LveDescriptorSetLayout(
            VVDevice &vvDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LveDescriptorSetLayout();
        LveDescriptorSetLayout(const LveDescriptorSetLayout &) = delete;
        LveDescriptorSetLayout &operator=(const LveDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        VVDevice &vvDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LveDescriptorWriter;
    };

    class VVDescriptorPool
    {
    public:
        class Builder
        {
        public:
            Builder(VVDevice &vvDevice) : vvDevice{vvDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<VVDescriptorPool> build() const;

        private:
            VVDevice &vvDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        VVDescriptorPool(
            VVDevice &VVDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~VVDescriptorPool();
        VVDescriptorPool(const VVDescriptorPool &) = delete;
        VVDescriptorPool &operator=(const VVDescriptorPool &) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        VVDevice &vvDevice;
        VkDescriptorPool descriptorPool;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorWriter
    {
    public:
        LveDescriptorWriter(LveDescriptorSetLayout &setLayout, VVDescriptorPool &pool);

        LveDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        LveDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        LveDescriptorSetLayout &setLayout;
        VVDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace VectorVertex