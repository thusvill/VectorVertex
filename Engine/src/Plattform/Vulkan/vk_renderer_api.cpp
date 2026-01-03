#include "vk_renderer_api.hpp"

#include <Render/FrameBuffer.hpp>
#include <Plattform/Vulkan/vk_framebuffer.hpp>
#include <Scene/Entity.hpp>

namespace VectorVertex
{
    VKRendererAPI::VKRendererAPI(Window *window) : m_Window(*window)
    {
        VV_CORE_INFO("Vulkan Render API Created!");
    }
    VKRendererAPI::~VKRendererAPI()
    {
        FreeCommandBuffers();
    }

    void VKRendererAPI::Init()
    {

        recreateSwapChain();
        CreateCommandBuffers();

        MaterialLibrary::InitMaterialLib();
        VVTextureLibrary::InitTextureLib();
        recreateRenderers();
        //
        // std::vector<VkDescriptorSetLayout> layout = {VKData.m_global_set_layout->getDescriptorSetLayout(), VVTextureLibrary::textureImageDescriptorLayout->getDescriptorSetLayout()};
        // VV_CORE_TRACE("Layouts befor create render syste: {}", layout.size());
        // MeshRenderSystem = CreateRef<VulkanMeshRenderer>(layout);
        // LightRenderSystem = CreateRef<VulkanLightRenderer>(layout);

        // LightRenderSystem = CreateRef<VulkanRenderSystem>(layout, sizeof(PointLightPushConstantData), "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.vert.spv", "/home/bios/CLionProjects/VectorVertex/VectorVertex/Resources/Shaders/point_light.frag.spv");
    }

    bool VKRendererAPI::BeginFrame()
    {
        VV_CORE_ASSERT(m_SwapChain != nullptr, "m_SwapChain is nullptr");

        auto result = m_SwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return false;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            VV_CORE_ERROR("Failed to acquire swap chain image!");
            throw std::runtime_error("Failed to acquire swap chain image!");
        }
        isFrameStarted = true;
        auto commandBuffer = VKGetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to begin recording command buffer!");
            throw std::runtime_error("Failed to begin recording command buffer!");
            return false;
        }
        else
        {
            return true;
        }
    }

    void VKRendererAPI::EndFrame()
    {
        auto commandBuffer = VKGetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to record command buffer!");
            throw std::runtime_error("Failed to record command buffer!");
        }

        auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized())
        {
            m_Window.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to present swap chain image!");
            throw std::runtime_error("Failed to present swap chain image!");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VKSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void VKRendererAPI::BeginRenderPass()
    {
        auto commandBuffer = VKGetCurrentCommandBuffer();
        assert(isFrameStarted && "Can't call BeginSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == VKGetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VKRendererAPI::EndRenderPass()
    {
        auto commandBuffer = VKGetCurrentCommandBuffer();

        assert(isFrameStarted && "Can't call EndSwapchainRenderPass if frame is not in progress!");
        assert(commandBuffer == VKGetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

    void VKRendererAPI::DedicateToFrameBuffer(FrameBuffer *framebuffer)
    {
        if (framebuffer != nullptr)
        {
            recreateSwapChain();
            CreateCommandBuffers();

            std::vector<VkDescriptorSetLayout> layout = {VKData.m_global_set_layout->getDescriptorSetLayout(), VVTextureLibrary::textureImageDescriptorLayout->getDescriptorSetLayout()};
            VV_CORE_TRACE("Layouts befor create render syste: {}", layout.size());
            VkRenderPass &renderpass = reinterpret_cast<VKFrameBuffer *>(framebuffer->GetFrameBufferAPI())->getRenderpass();
            MeshRenderSystem = CreateRef<VulkanMeshRenderer>(layout, *framebuffer);
            LightRenderSystem = CreateRef<VulkanLightRenderer>(layout, *framebuffer);
            m_attachedFrameBuffer = framebuffer;
            VV_CORE_INFO("Renderer Attached to a FrameBuffer");
        }else{
            VV_CORE_ERROR("Cant Attach to a null framebuffer!");
        }
    }

    void VKRendererAPI::DrawMesh(Entity object, FrameInfo info)
    {

        // MeshRenderSystem->Bind(object, info);
        //  auto m_data = object.GetComponent<MeshComponent>().GetMeshData();
        //  auto commandBuffer = VKGetCurrentCommandBuffer();
        //  std::vector<VkBuffer> buffers = {m_data.m_VertexBuffers->getVKBuffer()};

        // VkDeviceSize offsets[] = {0};
        // vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers.data(), offsets);
        // if (m_data.m_IndexCount > 0)
        // {
        //     vkCmdBindIndexBuffer(commandBuffer, m_data.m_IndexBuffer->getVKBuffer(), 0, VK_INDEX_TYPE_UINT32);
        //     vkCmdDrawIndexed(commandBuffer, m_data.m_IndexCount, 1, 0, 0, 0);
        // }
        // else
        // {
        //     vkCmdDraw(commandBuffer, m_data.m_VertexCount, 1, 0, 0);
        // }
    }

    void VKRendererAPI::UpdateObjects(std::unordered_map<UUID, Entity> objects, Entity *camera, FrameInfo &info)
    {
        if (!camera)
        {
            VV_CORE_ERROR("VKRendererAPI: Camera is Null");
        }
        LightRenderSystem->m_Camera = camera;

        LightRenderSystem->Update(objects, info);
    }

    void VKRendererAPI::DrawScene(std::unordered_map<UUID, Entity> objects, FrameInfo &info)
    {

        MeshRenderSystem->Render(objects, info);
        LightRenderSystem->Render(objects, info);
    }

    void *VKRendererAPI::GetSwapchain()
    {
        return m_SwapChain.get();
    }

    void *VKRendererAPI::GetRenderpass()
    {
        return m_SwapChain->getRenderPass();
    }

    int VKRendererAPI::GetCurrentFrameIndex()
    {
        return currentFrameIndex;
    }
    void VKRendererAPI::ClearResources()
    {

        recreateRenderers();
    }
    uint32_t VKRendererAPI::GetSwapchainImageCount()
    {
        return m_SwapChain->getImageCount();
    }

    void VKRendererAPI::CreateCommandBuffers()
    {
        auto &vkDevice = VKDevice::Get();
        commandBuffers.resize(VKSwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            VV_CORE_ERROR("Failed to allocate command buffers");
            throw std::runtime_error("Failed to create commandbuffers");
        }
    }

    void VKRendererAPI::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(VKDevice::Get().device(), VKDevice::Get().getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void VKRendererAPI::recreateSwapChain()
    {
        auto &vkDevice = VKDevice::Get();
        auto extent = m_Window.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = m_Window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vkDevice.device());
        if (m_SwapChain == nullptr)
        {
            m_SwapChain = std::make_unique<VKSwapChain>(vkDevice, extent);
        }
        else
        {
            std::shared_ptr<VKSwapChain> oldSwapChain = std::move(m_SwapChain);
            m_SwapChain = std::make_unique<VKSwapChain>(vkDevice, extent, oldSwapChain);
            if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get()))
            {
                VV_CORE_ERROR("Swap chain image(or depth) format does not match!");
                throw std::runtime_error("Swap chain image(or depth) format does not match!");
            }
        }
    }

    void VKRendererAPI::recreateRenderers()
    {
        VKData.Init();

        std::vector<VkDescriptorSetLayout> layout = {VKData.m_global_set_layout->getDescriptorSetLayout(), VVTextureLibrary::textureImageDescriptorLayout->getDescriptorSetLayout()};
        VV_CORE_TRACE("Layouts befor create render syste: {}", layout.size());
        if (m_attachedFrameBuffer == nullptr)
        {
            MeshRenderSystem = CreateRef<VulkanMeshRenderer>(layout);
            LightRenderSystem = CreateRef<VulkanLightRenderer>(layout);
        }
        else
        {
            
            VkRenderPass &renderpass = reinterpret_cast<VKFrameBuffer *>(m_attachedFrameBuffer->GetFrameBufferAPI())->getRenderpass();
            MeshRenderSystem = CreateRef<VulkanMeshRenderer>(layout, *m_attachedFrameBuffer);
            LightRenderSystem = CreateRef<VulkanLightRenderer>(layout, *m_attachedFrameBuffer);
            VV_CORE_INFO("Renderer Attached to a FrameBuffer");
        }
    }

} // namespace VectorVertex
