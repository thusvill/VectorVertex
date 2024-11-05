#pragma once
#include <vulkan/vulkan.h>

namespace VectorVertex
{
    class VKShader
    {
    public:
        VKShader() = default;

        VKShader(const std::string &filepath, VkShaderStageFlagBits stage) : shaderStage(stage)
        {
            auto shaderCode = readFile(filepath);
            createShaderModule(shaderCode);
        }
        ~VKShader()
        {
            if (shaderModule != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(VKDevice::Get().device(), shaderModule, nullptr);
            }
        }

        VkPipelineShaderStageCreateInfo getShaderStageInfo() const
        {
            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = shaderStage;
            shaderStageInfo.module = shaderModule;

            shaderStageInfo.pName = (shaderStage == VK_SHADER_STAGE_VERTEX_BIT) ? "vertex" : "fragment";

            return shaderStageInfo;
        }
        VkShaderModule getModule() const { return shaderModule; }

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