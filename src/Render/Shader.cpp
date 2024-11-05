#include "Shader.hpp"
#include <Renderer.hpp>
#include <vk_shader.hpp>

namespace VectorVertex{
    Ref<Shader> Shader::Create(const std::string &filepath)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateRef<VKShader>(filepath);
        }
        return nullptr;
    }
    Ref<Shader> Shader::Create(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            VV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateRef<VKShader>(name,  vertexSrc, fragmentSrc);
        }
        return nullptr;
    }
}