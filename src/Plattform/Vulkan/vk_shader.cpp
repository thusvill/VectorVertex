#include "vk_shader.hpp"
#include <shaderc/shaderc.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <vk_device.hpp>

namespace VectorVertex
{
    namespace Utils
    {

        static VkFlags VkShaderTypeFromString(const std::string &type)
        {
            if (type == "vertex")
                return VK_SHADER_STAGE_VERTEX_BIT;
            if (type == "fragment" || type == "pixel")
                return VK_SHADER_STAGE_FRAGMENT_BIT;

            VV_CORE_ASSERT(false, "Unknown shader type!");
            return 0;
        }
        static shaderc_shader_kind VkShaderStageToShaderC(VkFlags stage)
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

        static const char *VkShaderStageToString(VkFlags stage)
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

        std::string ReadFile(const std::string filepath)
        {

            std::string result;
            std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
            if (in)
            {
                in.seekg(0, std::ios::end);
                size_t size = in.tellg();
                if (size != -1)
                {
                    result.resize(size);
                    in.seekg(0, std::ios::beg);
                    in.read(&result[0], size);
                }
                else
                {
                    VV_CORE_ERROR("Could not read from file '{0}'", filepath);
                }
            }
            else
            {
                VV_CORE_ERROR("Could not open file '{0}'", filepath);
            }

            return result;
        }
    }

    VKShader::VKShader(const std::string &filepath)
    {
        Utils::CreateCacheDirectoryIfNeeded();

        std::string source = Utils::ReadFile(filepath);
        auto shaderSources = PreProcess(source);
    }

    std::unordered_map<VkFlags, std::string> VKShader::PreProcess(const std::string &source)
    {

        std::unordered_map<VkFlags, std::string> shaderSources;

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

        m_ShaderModules[stage] = shaderModule; // Store the shader module for later use
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
            std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

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
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
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

            // Create Vulkan shader module
            CreateShaderModule(stage, shaderData[stage]);
        }

        // // Reflect shader data (if needed)
        // for (const auto &[stage, data] : shaderData)
        // {
        //     Reflect(stage, data);
        // }
    }
}