#include "vk_shader.hpp"
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <vk_device.hpp>
#include <Log.h>
#include <Utils.hpp>
#include <RenderCommand.hpp>
#include <vk_api_data.hpp>
#include <vk_framebuffer.hpp>
namespace VectorVertex
{
    namespace Utils
    {

        static VkShaderStageFlagBits VkShaderTypeFromString(const std::string &type)
        {
            if (type == "vertex")
                return VK_SHADER_STAGE_VERTEX_BIT;
            if (type == "fragment" || type == "pixel")
                return VK_SHADER_STAGE_FRAGMENT_BIT;

            VV_CORE_ASSERT(false, "Unknown shader type!");
            return VK_SHADER_STAGE_VERTEX_BIT;
        }
        static shaderc_shader_kind VkShaderStageToShaderC(VkShaderStageFlagBits stage)
        {
            switch (stage)
            {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return shaderc_glsl_vertex_shader;
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return shaderc_glsl_fragment_shader;
            case VK_SHADER_STAGE_GEOMETRY_BIT:
                return shaderc_glsl_geometry_shader;
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
                return shaderc_glsl_tess_control_shader;
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
                return shaderc_glsl_tess_evaluation_shader;
            case VK_SHADER_STAGE_COMPUTE_BIT:
                return shaderc_glsl_compute_shader;
            default:
                assert(false && "Unsupported shader stage");
                return (shaderc_shader_kind)0; // Return an invalid shader kind
            }
        }

        static const char *VkShaderStageToString(VkShaderStageFlagBits stage)
        {
            switch (stage)
            {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return "VK_VERTEX_SHADER";
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return "VK_FRAGMENT_SHADER";
            case VK_SHADER_STAGE_GEOMETRY_BIT:
                return "VK_GEOMETRY_SHADER";
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
                return "VK_TESSELLATION_CONTROL_SHADER";
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
                return "VK_TESSELLATION_EVALUATION_SHADER";
            case VK_SHADER_STAGE_COMPUTE_BIT:
                return "VK_COMPUTE_SHADER";
            default:
                assert(false && "Unsupported shader stage");
                return "UNKNOWN_SHADER_STAGE";
            }
            VV_CORE_ASSERT(false);
            return nullptr;
        }

        static const char *GetCacheDirectory()
        {
            static std::string cacheDir = std::string(CACHE_DIR) + "/shader/opengl";
            return cacheDir.c_str();
        }

        static void CreateCacheDirectoryIfNeeded()
        {
            std::string cacheDirectory = GetCacheDirectory();
            if (!std::filesystem::exists(cacheDirectory))
                std::filesystem::create_directories(cacheDirectory);
        }

        static const char *VkShaderStageCachedVulkanFileExtension(uint32_t stage)
        {
            switch (stage)
            {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return ".cached_vulkan.vert";
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return ".cached_vulkan.frag";
            }
            VV_CORE_ASSERT(false);
            return "";
        }
    }

    VKShader::VKShader(const std::filesystem::path &filepath)
    {
        Utils::CreateCacheDirectoryIfNeeded();
        m_FilePath = filepath;

        std::string source = Utils::ReadFile(filepath);
        auto shaderSources = PreProcess(source);
        CompileAndCacheShaders(shaderSources);
        CreateProgramme();

        m_Name = filepath.filename();
        
    }

    VKShader::VKShader(const std::string &name, const std::filesystem::path &vertexSrc, const std::filesystem::path &fragmentSrc)
    {
        Utils::CreateCacheDirectoryIfNeeded();

        std::unordered_map<VkShaderStageFlagBits, std::string> sources;
        sources[VK_SHADER_STAGE_VERTEX_BIT] = vertexSrc;
        sources[VK_SHADER_STAGE_FRAGMENT_BIT] = fragmentSrc;

        CompileAndCacheShaders(sources);
        CreateProgramme();
        m_Name = name;
    }

    void VKShader::AttachToFramebuffer(FrameBuffer *framebuffer)
    {
        CreateProgramme(framebuffer);
    }

    VKShader::~VKShader()
    {
        for (auto &kv : m_ShaderModules)
        {
            auto &module = kv.second;
            vkDestroyShaderModule(VKDevice::Get().device(), module, nullptr);
        }
        m_ShaderModules.clear();
        vkDestroyBuffer(VKDevice::Get().device(), uniformBuffer, nullptr);
        vkFreeMemory(VKDevice::Get().device(), uniformBufferMemory, nullptr);
    }

    void VKShader::Bind()
    {
        auto commandbuffer = RenderCommand::GetRendererAPI()->GetCurrentCommandBuffer();
        m_Pipeline->Bind(commandbuffer);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, descriptorSet, 0, 1, &descriptorSet, 0, nullptr);
    
    }
    void VKShader::Unbind()
    {
        // No ubinding wanted for vulkan
    }

    std::unordered_map<VkShaderStageFlagBits, std::string> VKShader::PreProcess(const std::string &source)
    {

        std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

        const char *typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0); // Start of shader type declaration line
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
            VV_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
            std::string type = source.substr(begin, eol - begin);
            VV_CORE_ASSERT(Utils::VkShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
            VV_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

            shaderSources[Utils::VkShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }
    void VKShader::CreateShaderModule(VkShaderStageFlagBits stage, const std::vector<uint32_t> &code)
    {

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(VKDevice::Get().device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }

        m_ShaderModules[stage] = shaderModule;
    }
    void VKShader::CompileAndCacheShaders(const std::unordered_map<VkShaderStageFlagBits, std::string> &shaderSources)
    {

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        const bool optimize = true;
        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
        auto &shaderData = m_VulkanSPIRV;
        shaderData.clear();

        for (const auto &[stage, source] : shaderSources)
        {
            std::filesystem::path shaderFilePath = m_FilePath;
            std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::VkShaderStageCachedVulkanFileExtension(stage));

            // Attempt to read cached SPIR-V
            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (in.is_open())
            {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto &data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                in.read(reinterpret_cast<char *>(data.data()), size);
            }
            else
            {
                // Compile shader if cache miss
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::VkShaderStageToShaderC(stage), m_FilePath.c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    throw std::runtime_error("Shader compilation failed: " + std::string(module.GetErrorMessage()));
                }

                // Store compiled shader data
                shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

                // Cache the compiled shader
                std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                if (out.is_open())
                {
                    auto &data = shaderData[stage];
                    out.write(reinterpret_cast<const char *>(data.data()), data.size() * sizeof(uint32_t));
                }
            }
            CreateShaderModule(stage, shaderData[stage]);
        }
    }
    void VKShader::CreateProgramme(FrameBuffer *framebuffer)
    {
        VkPipelineLayout pipelineLayout;
        std::vector<VkDescriptorSetLayout> descriptorSetLayout = {VulkanAPIData::Get().m_global_set_layout->getDescriptorSetLayout(), VVTextureLibrary::textureImageDescriptorLayout->getDescriptorSetLayout()};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();

        if (vkCreatePipelineLayout(VKDevice::Get().device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        if (!framebuffer)
        {
            pipelineConfig.renderPass = reinterpret_cast<VkRenderPass>(RenderCommand::GetRendererAPI()->GetRenderpass());
        }
        else
        {
            for (int i = 1; i < framebuffer->GetSpecification().attachments.size(); i++)
            {
                VKPipeline::addAttachment(pipelineConfig, getVKFormat(framebuffer->GetSpecification().attachments[i]), false);
            }
            pipelineConfig.renderPass = reinterpret_cast<VKFrameBuffer *>(framebuffer->GetFrameBufferAPI())->getRenderpass();
        }
        pipelineConfig.pipelineLayout = pipelineLayout;
        m_Pipeline = CreateScope<VKPipeline>(pipelineConfig, this);

        CreateBuffer();
        AllocateMemory(VKDevice::Get().getPhysicalDevice());
    }

    void VKShader::CreateBuffer()
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferData.size();                   
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; 
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(VKDevice::Get().device(), &bufferInfo, nullptr, &uniformBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create uniform buffer!");
        }
    }

    void VKShader::AllocateMemory(VkPhysicalDevice physicalDevice)
    {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(VKDevice::Get().device(), uniformBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        // Find a suitable memory type
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (memRequirements.memoryTypeBits & (1 << i) &&
                (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
                (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                allocInfo.memoryTypeIndex = i;
                break;
            }
        }

        if (vkAllocateMemory(VKDevice::Get().device(), &allocInfo, nullptr, &uniformBufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate uniform buffer memory!");
        }

        vkBindBufferMemory(VKDevice::Get().device(), uniformBuffer, uniformBufferMemory, 0);
    }

    void VKShader::UpdateUniformBuffer()
    {
        void *data;
        vkMapMemory(VKDevice::Get().device(), uniformBufferMemory, 0, bufferData.size(), 0, &data);
        memcpy(data, bufferData.data(), bufferData.size());
        vkUnmapMemory(VKDevice::Get().device(), uniformBufferMemory);
    }

    void VKShader::SetInt(const std::string &name, int value)
    {
    }
    void VKShader::SetIntArray(const std::string &name, int *values, uint32_t count)
    {
    }
    void VKShader::SetFloat(const std::string &name, float value)
    {
    }
    void VKShader::SetFloat2(const std::string &name, const glm::vec2 &value)
    {
    }
    void VKShader::SetFloat3(const std::string &name, const glm::vec3 &value)
    {
    }
    void VKShader::SetFloat4(const std::string &name, const glm::vec4 &value)
    {
    }
    void VKShader::SetMat4(const std::string &name, const glm::mat4 &value)
    {
    }
}