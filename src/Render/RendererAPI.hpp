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
        std::vector<Ref<Buffer>> m_VertexBuffers;
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

        virtual void BeginFrame() = 0;      
        virtual void EndFrame() = 0;        
        virtual void BeginRenderPass() = 0; 
        virtual void EndRenderPass() = 0;   

        virtual void DrawMesh(MeshData data) = 0; 

        virtual void WaitForDeviceIdle() = 0; 

        static API GetRenderAPI()
        {
            return s_API;
        }
        static Scope<RendererAPI> Create(Window *window);

    private:
        static API s_API;
    };

} // namespace VectorVertex
