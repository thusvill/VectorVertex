#pragma once
#include <Core/vvpch.hpp>
#include <Base.h>
#include <Window.hpp>
#include <RendererAPI.hpp>
#include <GraphicsContext.hpp>
#include <vk_frame_info.hpp>

namespace VectorVertex
{

    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        static void Draw(Entity object, FrameInfo info);

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetRenderAPI(); }
    };

} // namespace VectorVertex
