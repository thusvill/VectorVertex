#pragma once
#include <Base.h>
#include <RendererAPI.hpp>
#include <Window.hpp>
#include <Entity.hpp>
namespace VectorVertex
{
    class RenderCommand
    {

    public:
        static void Init(Window *window)
        {
            s_RendererAPI = RendererAPI::Create(window, RendererAPI::API::Vulkan);

            s_RendererAPI->Init();
        }

        static bool BeginFrame()
        {
            return s_RendererAPI->BeginFrame();
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

        static void DrawMesh(Entity object, FrameInfo info)
        {
            s_RendererAPI->DrawMesh(object, info);
        }

        static void UpdateLights(std::unordered_map<UUID, Entity> objects, GlobalUBO ubo)
        {
            s_RendererAPI->UpdateLights(objects, ubo);
        }
        static void DrawLights(std::unordered_map<UUID, Entity> objects, Entity camera)
        {
            s_RendererAPI->DrawLights(objects, camera);
        }

        static void WindowResize()
        {
            s_RendererAPI->WindowResized();
        }

        static void WaitForDeviceIdl()
        {
            s_RendererAPI->WaitForDeviceIdle();
        }
        static Scope<RendererAPI> &GetRendererAPI()
        {
            return s_RendererAPI;
        }

    private:
        static Scope<RendererAPI> s_RendererAPI;
    };

} // namespace VectorVertex
