#pragma once
#include <Core/vvpch.hpp>
#include <Core/Base.h>

namespace VectorVertex
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void *getAPIClass() = 0;

        static Ref<Shader> CreateShader(std::filesystem::path vertex_path, std::filesystem::path fragment_path);
    };

} // namespace VectorVertex
