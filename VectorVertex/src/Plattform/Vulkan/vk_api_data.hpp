#pragma once
#include <vvpch.hpp>
#include <vulkan/vulkan.h>
#include <Plattform/Vulkan/vk_descriptors.hpp>
#include <Plattform/Vulkan/vk_buffer.hpp>
#include <Plattform/Vulkan/vk_texture.hpp>
#include <Plattform/Vulkan/vk_swap_chain.hpp>
#include <Plattform/Vulkan/vk_frame_info.hpp>
namespace VectorVertex
{

    class VulkanAPIData
    {
    public:
        VulkanAPIData();
        ~VulkanAPIData();
        bool Init();
        void Clear();
        Scope<VKDescriptorPool> m_global_pool;
        std::vector<Scope<VKBuffer>> m_ubo_buffers;
        std::vector<VkDescriptorSet> m_global_descriptor_sets;
        Scope<VKDescriptorSetLayout> m_global_set_layout;

        static VulkanAPIData &Get()
        {
            return *s_Instance;
        }

    private:
        static VulkanAPIData *s_Instance;
    };

} // namespace VectorVertex
