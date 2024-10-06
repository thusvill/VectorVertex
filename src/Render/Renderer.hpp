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
        static void Init();
        static void StartScene();
        static void EndScene();

        static void BeginFrame();
        static void EndFrame();

        static void WaitForIdle();
        
        inline static RendererAPI::API GetAPI() {return RendererAPI::GetAPI();}

    };
    

} // namespace VectorVertex
