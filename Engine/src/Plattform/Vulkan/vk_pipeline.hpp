#pragma once
#include <Core/vvpch.hpp>
#include "vk_device.hpp"
#include "vk_shader.hpp"

namespace VectorVertex
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
        std::vector<VkVertexInputBindingDescription> bind_descriptions{};

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{1}; // Array for two attachments
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class VKPipeline
    {
    public:
        VKPipeline(const PipelineConfigInfo &config_info, Shader *shader);
        ~VKPipeline();

        VKPipeline(const VKPipeline &) = delete;
        VKPipeline &operator=(const VKPipeline &) = delete;

        void Bind(VkCommandBuffer commandBUffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
        static void enableAlphaBlending(PipelineConfigInfo &configInfo, VkFormat format, int attachment);
        static void addAttachment(PipelineConfigInfo &configInfo, VkFormat format, bool blend = false);
        static VkColorComponentFlags getColorFormat(VkFormat format);

    private:
        static std::vector<char> readFile(const std::string &file_path);
        void CreateGraphicsPipeline(const PipelineConfigInfo &config_info, VKShader &shader);
        VkPipeline graphiscPipeline;
    };
} // namespace VectorVertex
