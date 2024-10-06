#pragma once
#include <Base.h>
#include <RendererAPI.hpp>

namespace VectorVertex
{
    class RenderCommand
    {

    public:
        static void Init()
        {

            s_RendererAPI->Init();
        }

        static void BeginFrame()
        {
            s_RendererAPI->BeginFrame();
        }
        static void EndFrame()
        {
            s_RendererAPI->EndFrame();
        }
        static void BeginRenderPass()
        {
            s_RendererAPI->BeginRenderPass();
        }
        static void EndRenderPass()
        {
            s_RendererAPI->EndRenderPass();
        }

        static void DrawMesh(MeshData data){
            s_RendererAPI->DrawMesh(data);
        }

    private:
        static Scope<RendererAPI> s_RendererAPI;
    };

} // namespace VectorVertex
