#pragma once
#include <Core/vvpch.hpp>
#include <Core/Base.h>
#include <glm/glm.hpp>
#include <Render/Buffer.hpp>
#include <Render/Window.hpp>
#include <Core/UUID.hpp>

#include <Plattform/Vulkan/vk_frame_info.hpp>

namespace VectorVertex
{
    class FrameBuffer;
    class Entity;
    struct MeshData
    {
        uint32_t m_IndexCount;
        uint32_t m_VertexCount;
        Ref<Buffer> m_VertexBuffers;
        Ref<Buffer> m_IndexBuffer;
    };

    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0,
            Vulkan = 1
        };

    public:
        virtual ~RendererAPI() = default;
        virtual void Init() = 0;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginRenderPass() = 0;
        virtual void EndRenderPass() = 0;

        virtual void DedicateToFrameBuffer(FrameBuffer *framebuffer) = 0;

        virtual void DrawMesh(Entity object, FrameInfo info) = 0;
        virtual void DrawScene(std::unordered_map<UUID, Entity> objects, FrameInfo &info) = 0;
        virtual void UpdateObjects(std::unordered_map<UUID, Entity> objects, Entity *camera, FrameInfo &info) = 0;

        virtual void WaitForDeviceIdle() = 0;

        virtual void *GetSwapchain() = 0;
        virtual void *GetRenderpass() = 0;
        virtual int GetCurrentFrameIndex() = 0;
        virtual uint32_t GetSwapchainImageCount() = 0;
        // virtual void* GetCurrentCommandBuffer() = 0;

        virtual VkCommandBuffer GetCurrentCommandBuffer() = 0;

        virtual void WindowResized() = 0;

        virtual void ClearResources() = 0;

        static API GetRenderAPI()
        {
            return s_API;
        }
        static Scope<RendererAPI> Create(Window *window, API api = API::Vulkan);

    private:
        static API s_API;
    };

} // namespace VectorVertex
