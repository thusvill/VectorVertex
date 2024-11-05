#pragma once
#include <vulkan/vulkan.h>
#include <Shader.hpp>

namespace VectorVertex
{
    class VKShader : public Shader
    {
    public:
        VKShader(const std::string &filepath);
        VKShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);

        virtual ~VKShader();
        virtual void Bind() override;
        virtual void Unbind() override;
        virtual void SetInt(const std::string &name, int value) override;
        virtual void SetIntArray(const std::string &name, int *values, uint32_t count) override;
        virtual void SetFloat(const std::string &name, float value) override;
        virtual void SetFloat2(const std::string &name, const glm::vec2 &value) override;
        virtual void SetFloat3(const std::string &name, const glm::vec3 &value) override;
        virtual void SetFloat4(const std::string &name, const glm::vec4 &value) override;
        virtual void SetMat4(const std::string &name, const glm::mat4 &value) override;
        
        private:
            std::unordered_map<VkFlags, std::string> PreProcess(const std::string &source);

            std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> m_VulkanSPIRV;
            std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;

            void CreateShaderModule(VkShaderStageFlagBits stage, const std::vector<uint32_t> &code);
            void CompileAndCacheShaders(const std::unordered_map<VkShaderStageFlagBits, std::string> &shaderSources);
            
    }