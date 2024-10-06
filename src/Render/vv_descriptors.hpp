#pragma once

#include "vv_device.hpp"
#include <vvpch.hpp>

namespace VectorVertex
{

    class VVDescriptorSetLayout
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
            std::unique_ptr<VVDescriptorSetLayout> build() const;

        private:
            VVDevice &vvDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        VVDescriptorSetLayout(
            VVDevice &vvDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~VVDescriptorSetLayout();
        VVDescriptorSetLayout(const VVDescriptorSetLayout &) = delete;
        VVDescriptorSetLayout &operator=(const VVDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout()  { return descriptorSetLayout; }

    private:
        VVDevice &vvDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class VVDescriptorWriter;
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

        VkDescriptorPool getPool() {return descriptorPool;}

        uint32_t maxSets;

    private:
        VVDevice &vvDevice;
        VkDescriptorPool descriptorPool;

        friend class VVDescriptorWriter;
    };

    class VVDescriptorWriter
    {
    public:
        VVDescriptorWriter(VVDescriptorSetLayout &setLayout, VVDescriptorPool &pool);

        VVDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        VVDescriptorWriter &writeBuffer(uint32_t binding,VkDescriptorSet descriptorSet ,VkDescriptorBufferInfo *bufferInfo);
        VVDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
        VVDescriptorWriter &writeImage(uint32_t binding, VkDescriptorSet descriptorSet, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        VVDescriptorSetLayout &setLayout;
        VVDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace VectorVertex