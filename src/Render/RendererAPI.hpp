#pragma once
#include <vvpch.hpp>
#include <Base.h>
#include <glm/glm.hpp>
#include <Buffer.hpp>
#include <Window.hpp>
namespace VectorVertex
{
    struct MeshData
    {
        uint32_t m_IndexCount;
        uint32_t m_VertexCount;
        std::vector<Buffer> m_VertexBuffers;
        Buffer m_IndexBuffer;
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

        virtual void BeginFrame();
        virtual void EndFrame();
        virtual void BeginRenderPass();
        virtual void EndRenderPass();

        virtual void DrawMesh(MeshData data);

        virtual void WaitForDeviceIdle();

        static API GetAPI()
        {
            return s_API;
        }
        static Scope<RendererAPI> Create(Window *window);

    private:
        static API s_API;
    };

} // namespace VectorVertex
