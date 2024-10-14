#pragma once
#include <vvpch.hpp>
#include <Base.h>
#include <Window.hpp>
#include <RendererAPI.hpp>
#include <GraphicsContext.hpp>
namespace VectorVertex
{

    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        static void Draw(MeshData data);

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetRenderAPI(); }
    };

} // namespace VectorVertex
