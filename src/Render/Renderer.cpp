#include "Renderer.hpp"
#include <RenderCommand.hpp>

namespace VectorVertex
{
    void Renderer::Draw(Entity object, FrameInfo info)
    {
        RenderCommand::DrawMesh(object, info);
    }
}