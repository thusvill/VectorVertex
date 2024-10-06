#pragma once
#include <vulkan/vulkan.h>
#include <vvpch.hpp>
#include <VectorVertex.hpp>

namespace VectorVertex
{
    class VVShader
    {
    public:
        VVShader() = default;
        VVShader(const std::string &filepath, VkShaderStageFlagBits stage) :shaderStage(stage)
        {
            auto shaderCode = readFile(filepath);
            createShaderModule(shaderCode);
        }
        ~VVShader()
        {
            if (shaderModule != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(Application::Get().GetDevice().device(), shaderModule, nullptr);
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
        VkShaderModule getModule() const { return shaderModule;}

    private:
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        VkShaderStageFlagBits shaderStage;

        std::vector<char> readFile(const std::string &filepath)
        {
            std::ifstream file(filepath, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                throw std::runtime_error("failed to open file: " + filepath);
            }

            size_t fileSize = static_cast<size_t>(file.tellg());
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();
            return buffer;
        }

        void createShaderModule(const std::vector<char> &code)
        {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

            if (vkCreateShaderModule(Application::Get().GetDevice().device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create shader module!");
            }
        }
    };
}