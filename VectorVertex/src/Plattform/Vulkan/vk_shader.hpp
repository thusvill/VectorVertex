#pragma once
#include <vulkan/vulkan.h>
#include <Render/Shader.hpp>
#include <Plattform/Vulkan/vk_device.hpp>
namespace VectorVertex
{
    struct VulkanShader
    {
        VulkanShader(const std::string Name):name(Name){}
        const std::string name;
        VkShaderStageFlagBits stage;
        VkShaderModule module;
    };
    class VKShader : public Shader
    {
    public:
        VKShader(std::filesystem::path vertex_path, std::filesystem::path fragment_path);

        virtual void Bind() override;
        virtual void *getAPIClass() override;

        virtual ~VKShader() override
        {
            if (m_Shaders.size() < 0)
            {
                for (int i = 0; i < m_Shaders.size(); i++)
                {
                    vkDestroyShaderModule(VKDevice::Get().device(), m_Shaders[i].module, nullptr);
                    VV_CORE_INFO("Deleted Shader: {}", m_Shaders[i].name);
                }
            }
        }

        std::vector<VulkanShader> getModules() const { return m_Shaders; }

    private:
        void Load(std::unordered_map<VkShaderStageFlagBits, std::filesystem::path> sources);
        void CompileShader(VkShaderStageFlagBits stage, std::filesystem::path path);

        std::vector<VulkanShader> m_Shaders;

        std::string LoadShaderSource(const std::string &filepath)
        {
            std::ifstream file(filepath, std::ios::ate | std::ios::binary);
            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open file: " + filepath);
            }
            size_t fileSize = static_cast<size_t>(file.tellg());
            std::string buffer(fileSize, '\0'); // Initialize string with file size
            file.seekg(0);
            file.read(&buffer[0], fileSize); // Read into string data
            file.close();
            return buffer;
        }
        VkShaderModule createShaderModule(const std::vector<uint8_t> &code)
        {
            VkShaderModule shaderModule;
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

            if (vkCreateShaderModule(VKDevice::Get().device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create shader module!");
            }
            return shaderModule;
        }
    };
}