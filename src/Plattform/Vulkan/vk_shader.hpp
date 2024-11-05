#pragma once
#include <vulkan/vulkan.h>
<<<<<<< HEAD
#include <vk_pipeline.hpp>
#include <Shader.hpp>
    == == ==
    =
#include <vvpch.hpp>

        >>>>>>> parent of 4b0a992(befor change shaders)

                    namespace VectorVertex
{
    class VKShader
    {
    public:
        VKShader(const std::filesystem::path &filepath);

        virtual void AttachToFramebuffer(FrameBuffer *framebuffer) override;
        virtual const std::string &GetName() const override { return m_Name; }

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

        std::unordered_map<VkShaderStageFlagBits, VkShaderModule> &GetModules() { return m_ShaderModules; }

    private:
        std::filesystem::path m_FilePath;
        std::string m_Name;
        Scope<VKPipeline> m_Pipeline;

        std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>
            m_VulkanSPIRV;
        std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;

        std::unordered_map<VkShaderStageFlagBits, std::string> PreProcess(const std::string &source);
        void CreateShaderModule(VkShaderStageFlagBits stage, const std::vector<uint32_t> &code);
        void CompileAndCacheShaders(const std::unordered_map<VkShaderStageFlagBits, std::string> &shaderSources);
        void CreateProgramme(FrameBuffer *framebuffer = nullptr);

    private:
        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        std::unordered_map<std::string, size_t> uniformOffsets;
        std::vector<uint8_t> bufferData;
        void CreateBuffer();
        void AllocateMemory(VkPhysicalDevice physicalDevice);
        void UpdateUniformBuffer();
    };
}