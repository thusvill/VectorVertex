#pragma once
#include <RendererAPI.hpp>
#include <vvpch.hpp>
#include <Log.h>
#include "vk_window.hpp"
#include "vk_device.hpp"
#include "vk_swap_chain.hpp"
#include <Window.hpp>
#include <Buffer.hpp>

namespace VectorVertex
{
    class VKRendererAPI : public RendererAPI
    {
    public:
        VKRendererAPI(Window* window);
        virtual ~VKRendererAPI() override
        {
            FreeCommandBuffers();
        }
        virtual void Init() override;
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void BeginRenderPass() override;
        virtual void EndRenderPass() override;

        virtual void DrawMesh(MeshData data) override;

    private:
        void
        CreateCommandBuffers();
        void FreeCommandBuffers();

        void recreateSwapChain();

        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            VV_CORE_ASSERT(isFrameStarted, "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        std::unique_ptr<VKSwapChain> m_SwapChain;

        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};

        Window& m_Window;
    };
} // namespace VectorVertex
