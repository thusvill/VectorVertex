#include "vk_api_data.hpp"

namespace VectorVertex
{

    VulkanAPIData *VulkanAPIData::s_Instance = nullptr;
    VulkanAPIData::VulkanAPIData()
    {
        VV_CORE_ASSERT(!s_Instance, "Vulkan API Data already exists!");
        s_Instance = this;
    }

    VulkanAPIData::~VulkanAPIData()
    {
        Clear();
        m_global_pool->freeDescriptors(m_global_descriptor_sets);
        m_global_pool->~VKDescriptorPool();
        
        }

    bool VulkanAPIData::Init()
    {
        Clear();
        VVTextureLibrary::Reset();
        VVTextureLibrary::UpdateDescriptors();
        m_global_pool = VKDescriptorPool::Builder(VKDevice::Get())
                            .setMaxSets(VKSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VKSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                            //.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VKSwapChain::MAX_FRAMES_IN_FLIGHT)
                            .build();

        m_ubo_buffers.resize(VKSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < m_ubo_buffers.size(); i++)
        {
            m_ubo_buffers[i] = std::make_unique<VKBuffer>(sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            m_ubo_buffers[i]->map();
        }

        m_global_set_layout = VKDescriptorSetLayout::Builder(VKDevice::Get())
                                  .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                  //.addBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                  .build();

        m_global_descriptor_sets.resize(VKSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_global_descriptor_sets.size(); i++)
        {
            auto buffer_info = m_ubo_buffers[i]->descriptorInfo();

            VKDescriptorWriter(*m_global_set_layout, *m_global_pool)
                .writeBuffer(0, &buffer_info)
                // add texture sampler here
                .build(m_global_descriptor_sets[i]);
        }
        VV_CORE_INFO("Vulkan API Data Initialized!");
        return true;
    }
    void VulkanAPIData::Clear()
    {
        if (m_global_pool != nullptr)
        {
            m_global_pool->freeDescriptors(m_global_descriptor_sets);
            m_global_pool->resetPool();
        }
        
        m_global_descriptor_sets.clear();
        m_ubo_buffers.clear();

        
        // VVTextureLibrary::ClearLibrary();
        VV_CORE_INFO("Global Resources Cleared!");
    }
} // namespace VectorVertex
