#pragma once
#include <vvpch.hpp>
#include <Base.h>
#include <glm/glm.hpp>

namespace VectorVertex
{
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

        static API GetAPI() { return s_API; }
        static Scope<RendererAPI> Create(Window* window);

    private:
        static API s_API;
    };

} // namespace VectorVertex
