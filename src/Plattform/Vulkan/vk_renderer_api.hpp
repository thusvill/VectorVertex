#pragma once
#include <vulkan/vulkan.h>
#include <RendererAPI.hpp>
#include <vvpch.hpp>
#include <Log.h>
#include "vk_window.hpp"
#include "vk_device.hpp"
#include "vk_swap_chain.hpp"
#include <Window.hpp>
#include <Buffer.hpp>
#include <vk_api_data.hpp>
#include <vk_render_system.hpp>
#include <vk_mesh_render_system.hpp>
#include <vk_texture.hpp>

namespace VectorVertex
{
    class Entity;
    class VKRendererAPI : public RendererAPI
    {
    public:
        VKRendererAPI(Window *window);
        virtual ~VKRendererAPI() override;
        virtual void Init() override;
        virtual bool BeginFrame() override;
        virtual void EndFrame() override;
        virtual void BeginRenderPass() override;
        virtual void EndRenderPass() override;

        virtual void DrawMesh(Entity object, FrameInfo info) override;
        virtual void DrawScene(std::unordered_map<UUID, Entity> objects, FrameInfo info) override;

        virtual void *GetSwapchain() override;
        virtual void *GetRenderpass() override;
        virtual int GetCurrentFrameIndex() override;
        virtual uint32_t GetSwapchainImageCount() override;

        virtual VkCommandBuffer GetCurrentCommandBuffer() override
        {
            return VKGetCurrentCommandBuffer();
        }

        virtual void WaitForDeviceIdle() override
        {
            vkDeviceWaitIdle(VKDevice::Get().device());
        }

        virtual void WindowResized() override
        {
            recreateSwapChain();
        }

    private:
        VulkanAPIData VKData{};
        Ref<VulkanMeshRenderer> MeshRenderSystem;

        void UploadShaderData(Entity entity);
        void
        CreateCommandBuffers();
        void FreeCommandBuffers();

        void recreateSwapChain();

        VkCommandBuffer VKGetCurrentCommandBuffer() const
        {
            VV_CORE_ASSERT(isFrameStarted, "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        std::unique_ptr<VKSwapChain> m_SwapChain;

        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};

        Window &m_Window;
    };
} // namespace VectorVertex
