#pragma once
#include <vvpch.hpp>
#include <Base.h>
#include <filesystem>

namespace VectorVertex
{
    class Shader
    {
    public:
        virtual ~Shader() = default;
        virtual void AttachToFramebuffer(FrameBuffer *framebuffer) = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetInt(const std::string &name, int value) = 0;
        virtual void SetIntArray(const std::string &name, int *values, uint32_t count) = 0;
        virtual void SetFloat(const std::string &name, float value) = 0;
        virtual void SetFloat2(const std::string &name, const glm::vec2 &value) = 0;
        virtual void SetFloat3(const std::string &name, const glm::vec3 &value) = 0;
        virtual void SetFloat4(const std::string &name, const glm::vec4 &value) = 0;
        virtual void SetMat4(const std::string &name, const glm::mat4 &value) = 0;

        virtual const std::string &GetName() const = 0;
        static Ref<Shader> Create(const std::filesystem::path &filepath);
        static Ref<Shader> Create(const std::string &name, const std::filesystem::path &vertexSrc, const std::filesystem::path &fragmentSrc);
    };

}
