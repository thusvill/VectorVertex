#include "Renderer.hpp"
#include <RenderCommand.hpp>

namespace VectorVertex
{
    void Renderer::Draw(MeshData data)
    {
        RenderCommand::DrawMesh(data);
    }
}