#pragma once
#include <Base.h>
#include <RendererAPI.hpp>
#include <Window.hpp>
namespace VectorVertex
{
    class RenderCommand
    {

    public:
        static void Init(Window *window)
        {
            s_RendererAPI = RendererAPI::Create(window);

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

        static void DrawMesh(MeshData data)
        {
            s_RendererAPI->DrawMesh(data);
        }

        static void WaitForDeviceIdl(){
            s_RendererAPI->WaitForDeviceIdle();
        }

    private:
        static Scope<RendererAPI> s_RendererAPI;
    };

} // namespace VectorVertex
