#include "vk_shader.hpp"
#include <spirv-tools/optimizer.hpp>
#include <spirv-tools/libspirv.h>
#include <shaderc/shaderc.hpp>
namespace VectorVertex
{
    namespace Utils
    {
        void ValidateSPIRV(const std::vector<uint32_t> &spirvBinary)
        {
            spvtools::SpirvTools tools(SPV_ENV_VULKAN_1_0);
            if (!tools.Validate(spirvBinary.data(), spirvBinary.size()))
            {
                throw std::runtime_error("SPIR-V validation failed");
            }
        }
        void WriteToFile(const std::filesystem::path &filepath, const std::vector<uint32_t> &binaryData)
        {
            try
            {
                std::filesystem::create_directories(filepath.parent_path());

                // Open the file for writing in binary mode
                std::ofstream file(filepath, std::ios::out | std::ios::binary);

                if (!file)
                {
                    throw std::runtime_error("Failed to open file: " + filepath.string());
                }

                // Write the binary data to the file
                file.write(reinterpret_cast<const char *>(binaryData.data()), binaryData.size() * sizeof(uint32_t));

                file.close();
                std::cout << "SPIR-V binary written to: " << filepath << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
        void CreateDirIfNotExisist(std::filesystem::path path)
        {
            try
            {
                if (!std::filesystem::create_directories(path))
                {
                    std::cout << "Directory already exists or created successfully: " << path << std::endl;
                }
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                throw std::runtime_error("Failed to create directory tree: " + std::string(e.what()));
            }
        }
        std::string ShaderStageToString(VkShaderStageFlagBits stage)
        {
            switch (stage)
            {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return "vertex";
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return "fragemnt";
            }
        }
        shaderc_shader_kind ShaderStageToShadercKind(VkShaderStageFlagBits stage)
        {
            switch (stage)
            {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return shaderc_shader_kind::shaderc_vertex_shader;
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return shaderc_shader_kind::shaderc_fragment_shader;
            }
        }
        std::vector<uint8_t> LoadSPIRVBinary(const std::string &filepath)
        {
            std::ifstream file(filepath, std::ios::ate | std::ios::binary);
            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open SPIR-V file: " + filepath);
            }

            size_t fileSize = static_cast<size_t>(file.tellg());
            file.seekg(0);

            std::cout << "File size: " << fileSize << " bytes" << std::endl;

            if (fileSize % 4 != 0)
            {
                throw std::runtime_error("SPIR-V file size is not a multiple of 4: " + filepath);
            }

            std::vector<uint8_t> buffer(fileSize);

            file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
            if (!file)
            {
                throw std::runtime_error("Failed to read SPIR-V file: " + filepath);
            }

            file.close();
            return buffer;
        }
    }
    VKShader::VKShader(std::filesystem::path vertex_path, std::filesystem::path fragment_path)
    {
        std::unordered_map<VkShaderStageFlagBits, std::filesystem::path> paths{2};
        paths[VK_SHADER_STAGE_VERTEX_BIT] = vertex_path;
        paths[VK_SHADER_STAGE_FRAGMENT_BIT] = fragment_path;

        Load(paths);
    }

    void VKShader::Bind()
    {
    }

    void *VKShader::getAPIClass()
    {
        return this;
    }

    void VKShader::Load(std::unordered_map<VkShaderStageFlagBits, std::filesystem::path> sources)
    {
        for (auto &source : sources)
        {
            std::filesystem::path targetPath = std::filesystem::path(CACHE_DIR) / "shaders" / "vulkan" / (source.second.filename().string() + "." + Utils::ShaderStageToString(source.first));

            if (!std::filesystem::exists(targetPath))
            {

                CompileShader(source.first, source.second);
            }
            VulkanShader shader(source.second.stem().string());

            shader.stage = source.first;
            shader.module = createShaderModule(Utils::LoadSPIRVBinary(targetPath));
            VV_CORE_INFO("Shader Loaded: {}", shader.name);
            m_Shaders.push_back(shader);
        }
    }

    void VKShader::CompileShader(VkShaderStageFlagBits stage, std::filesystem::path path)
    {
        std::filesystem::path cached_path = std::filesystem::path(CACHE_DIR) / "shaders" / "vulkan/";
        Utils::CreateDirIfNotExisist(cached_path);
        std::string source = LoadShaderSource(path);
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, Utils::ShaderStageToShadercKind(stage), path.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            throw std::runtime_error("Shader compilation failed: " + result.GetErrorMessage());
        }
        else
        {
            const std::vector<uint32_t> &spirvBinary = std::vector<uint32_t>(
                result.cbegin(), result.cend());
            cached_path /= path.filename().string() + "." + Utils::ShaderStageToString(stage);
            Utils::WriteToFile(cached_path, spirvBinary);
            Utils::ValidateSPIRV(spirvBinary);
            VV_CORE_INFO("Shader Compiled : {}", cached_path.string());
        }
    }

} // namespace VectorVerte
