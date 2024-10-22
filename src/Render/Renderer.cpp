#include "Renderer.hpp"
#include <RenderCommand.hpp>

namespace VectorVertex
{
    void Renderer::Draw(Entity object)
    {
        RenderCommand::DrawMesh(object);
    }
}