#include "vv_descriptors.hpp"
#include <Log.h>

// std
#include <cassert>
#include <stdexcept>

namespace VectorVertex
{

    // *************** Descriptor Set Layout Builder *********************

    VVDescriptorSetLayout::Builder &VVDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count)
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<VVDescriptorSetLayout> VVDescriptorSetLayout::Builder::build() const
    {
        return std::make_unique<VVDescriptorSetLayout>(vvDevice, bindings);
    }

    // *************** Descriptor Set Layout *********************

    VVDescriptorSetLayout::VVDescriptorSetLayout(
        VVDevice &vvDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : vvDevice{vvDevice}, bindings{bindings}
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
                vvDevice.device(),
                &descriptorSetLayoutInfo,
                nullptr,
                &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    VVDescriptorSetLayout::~VVDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(vvDevice.device(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    VVDescriptorPool::Builder &VVDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count)
    {
        poolSizes.push_back({descriptorType, count});
        return *this;
    }

    VVDescriptorPool::Builder &VVDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags)
    {
        poolFlags = flags;
        return *this;
    }
    VVDescriptorPool::Builder &VVDescriptorPool::Builder::setMaxSets(uint32_t count)
    {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<VVDescriptorPool> VVDescriptorPool::Builder::build() const
    {
        return std::make_unique<VVDescriptorPool>(vvDevice, maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    VVDescriptorPool::VVDescriptorPool(
        VVDevice &vvDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes)
        : vvDevice{vvDevice}
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(vvDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    VVDescriptorPool::~VVDescriptorPool()
    {
        vkDestroyDescriptorPool(vvDevice.device(), descriptorPool, nullptr);
    }

    bool VVDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(vvDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void VVDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const
    {
        vkFreeDescriptorSets(
            vvDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void VVDescriptorPool::resetPool()
    {
        vkResetDescriptorPool(vvDevice.device(), descriptorPool, 0);
    
    }


    VVDescriptorWriter::VVDescriptorWriter(VVDescriptorSetLayout &setLayout, VVDescriptorPool &pool)
        : setLayout{setLayout}, pool{pool} {}

    VVDescriptorWriter &VVDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo *bufferInfo)
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    VVDescriptorWriter &VVDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorSet descriptorSet, VkDescriptorBufferInfo *bufferInfo)
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstSet = descriptorSet;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    VVDescriptorWriter &VVDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo *imageInfo)
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    VVDescriptorWriter &VVDescriptorWriter::writeImage(uint32_t binding, VkDescriptorSet descriptorSet, VkDescriptorImageInfo *imageInfo)
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstSet = descriptorSet;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool VVDescriptorWriter::build(VkDescriptorSet &set)
    {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success)
        {
            VV_CORE_ERROR("Cannot Allocate Descriptor set!");

            
            pool.resetPool();
            success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
            if (!success)
            {
                VV_CORE_ERROR("Cannot allocate descriptor set after reset!");
                return false;
            }else{
                VV_CORE_WARN("Created allocate descriptor set after reset!");
               
            }
        }
        overwrite(set);
        return true;
    }

    void VVDescriptorWriter::overwrite(VkDescriptorSet &set)
    {
        for (auto &write : writes)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.vvDevice.device(), writes.size(), writes.data(), 0, nullptr);
    }

} // namespace VectorVertex