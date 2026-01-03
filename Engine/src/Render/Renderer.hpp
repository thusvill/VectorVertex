#pragma once
#include <Core/vvpch.hpp>
#include <Core/Base.h>
#include <Render/Window.hpp>
#include <Render/RendererAPI.hpp>
#include <Render/GraphicsContext.hpp>
#include <Plattform/Vulkan/vk_frame_info.hpp>

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
