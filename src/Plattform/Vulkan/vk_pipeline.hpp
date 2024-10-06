#pragma once
#include <vvpch.hpp>
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
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
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
        VKPipeline(const PipelineConfigInfo &config_info, const std::string vertex_shader, const std::string fragment_shader);
        ~VKPipeline();

        VKPipeline(const VKPipeline &) = delete;
        VKPipeline &operator=(const VKPipeline &) = delete;

        void Bind(VkCommandBuffer commandBUffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
        static void enableAlphaBlending(PipelineConfigInfo &configInfo);

    private:
        static std::vector<char> readFile(const std::string &file_path);
        void CreateGraphicsPipeline(const PipelineConfigInfo &config_info, const std::string vertex_shader, const std::string fragment_shader);
        VkPipeline graphiscPipeline;
    };
} // namespace VectorVertex
