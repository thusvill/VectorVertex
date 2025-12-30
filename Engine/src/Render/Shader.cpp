#include "Shader.hpp"
#include <Renderer.hpp>
#include <vk_shader.hpp>

namespace VectorVertex
{
    Ref<Shader> Shader::CreateShader(std::filesystem::path vertex_path, std::filesystem::path fragment_path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::Vulkan:
            return CreateRef<VKShader>(vertex_path, fragment_path);
            break;

            return nullptr;
        }
    }
}