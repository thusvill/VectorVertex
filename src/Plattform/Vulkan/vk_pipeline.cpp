#include "vk_pipeline.hpp"
#include "vk_model.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <Application.hpp>
namespace VectorVertex
{
    VKPipeline::VKPipeline(const PipelineConfigInfo &config_info, Shader *shader)
    {
        VKShader *vkshader = static_cast<VKShader *>(shader->getAPIClass());

        if (!vkshader)
        {
            throw std::runtime_error("Shader is not a valid VKShader.");
        }

        CreateGraphicsPipeline(config_info, *vkshader);
    }

    VKPipeline::~VKPipeline()
    {
        vkDestroyPipeline(VKDevice::Get().device(), graphiscPipeline, nullptr);
    }

    void VKPipeline::Bind(VkCommandBuffer commandBUffer)
    {
        vkCmdBindPipeline(commandBUffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphiscPipeline);
    }

    void VKPipeline::defaultPipelineConfigInfo(PipelineConfigInfo &configInfo)
    {

        configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        configInfo.viewportInfo.viewportCount = 1;
        configInfo.viewportInfo.pViewports = nullptr;
        configInfo.viewportInfo.scissorCount = 1;
        configInfo.viewportInfo.pScissors = nullptr;

        configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.rasterizationInfo.lineWidth = 1.0f;
        configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        configInfo.multisampleInfo.minSampleShading = 1.0f;          // Optional
        configInfo.multisampleInfo.pSampleMask = nullptr;            // Optional
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

        // configInfo.colorBlendAttachments[0].colorWriteMask =
        //     VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        //     VK_COLOR_COMPONENT_A_BIT;
        // configInfo.colorBlendAttachments[0].blendEnable = VK_FALSE;
        // configInfo.colorBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        // configInfo.colorBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        // configInfo.colorBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        // configInfo.colorBlendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        // configInfo.colorBlendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        // configInfo.colorBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD;             // Optional
        configInfo.colorBlendInfo.attachmentCount = 0;
        configInfo.colorBlendInfo.pAttachments = configInfo.colorBlendAttachments.data();

        configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        configInfo.colorBlendInfo.blendConstants[0] = 0.0f;   // Optional
        configInfo.colorBlendInfo.blendConstants[1] = 0.0f;   // Optional
        configInfo.colorBlendInfo.blendConstants[2] = 0.0f;   // Optional
        configInfo.colorBlendInfo.blendConstants[3] = 0.0f;   // Optional

        configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
        configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
        configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.front = {}; // Optional
        configInfo.depthStencilInfo.back = {};  // Optional

        configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
        configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
        configInfo.dynamicStateInfo.flags = 0;

        configInfo.attribute_descriptions = VKModel::Vertex::getAttributeDescriptions();
        configInfo.bind_descriptions = VKModel::Vertex::getBindingDescriptions();
    }

    void VKPipeline::enableAlphaBlending(PipelineConfigInfo &configInfo, VkFormat format, int attachment)
    {

        configInfo.colorBlendAttachments[attachment].blendEnable = VK_TRUE;
        configInfo.colorBlendAttachments[attachment].colorWriteMask = getColorFormat(format);
        configInfo.colorBlendAttachments[attachment].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        configInfo.colorBlendAttachments[attachment].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        configInfo.colorBlendAttachments[attachment].colorBlendOp = VK_BLEND_OP_ADD;
        configInfo.colorBlendAttachments[attachment].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        configInfo.colorBlendAttachments[attachment].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        configInfo.colorBlendAttachments[attachment].alphaBlendOp = VK_BLEND_OP_ADD;
    }

    void VKPipeline::addAttachment(PipelineConfigInfo &configInfo, VkFormat format, bool blend)
    {
        VkPipelineColorBlendAttachmentState color_attachment;

        color_attachment.blendEnable = (blend) ? VK_TRUE : VK_FALSE;
        color_attachment.colorWriteMask = getColorFormat(format);
        color_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        configInfo.colorBlendAttachments.push_back(color_attachment);
        configInfo.colorBlendInfo.attachmentCount = configInfo.colorBlendAttachments.size();
        configInfo.colorBlendInfo.pAttachments = configInfo.colorBlendAttachments.data();
    }
    VkColorComponentFlags VKPipeline::getColorFormat(VkFormat format)
    {
        VkColorComponentFlags flags;
        switch (format)
        {
        case VK_FORMAT_R8G8B8_UNORM:
            flags = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            break;

        case VK_FORMAT_R32_SINT:
            flags = VK_COLOR_COMPONENT_R_BIT;
            break;

        default:
            flags = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            break;
        }
        return flags;
    }

    std::vector<char> VKPipeline::readFile(const std::string &file_path)
    {
        std::ifstream file{file_path, std::ios::ate | std::ios::binary};
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();
        return buffer;
    }

    void VKPipeline::CreateGraphicsPipeline(const PipelineConfigInfo &config_info, VKShader &shader)
    {
        assert(config_info.pipelineLayout != VK_NULL_HANDLE && "Cannot create Graphics pipeline :: no Pipelinelayout provided in config");
        assert(config_info.renderPass != VK_NULL_HANDLE && "Cannot create Graphics pipeline :: no Renderpass provided in config");

        // VKShader v_shader(vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
        // VKShader f_shader(fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
        shaderStages.reserve(shader.getModules().size());
        for (auto &module : shader.getModules())
        {
            VkPipelineShaderStageCreateInfo shaderStage{};
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = module.stage;
            shaderStage.module = module.module;
            shaderStage.pName = "main";
            shaderStage.flags = 0;
            shaderStage.pNext = nullptr;
            shaderStage.pSpecializationInfo = nullptr;

            shaderStages.push_back(shaderStage);
        }

        auto &bindingDescriptions = config_info.bind_descriptions;
        auto &attributeDescriptions = config_info.attribute_descriptions;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &config_info.inputAssemblyInfo;
        pipelineInfo.pViewportState = &config_info.viewportInfo;
        pipelineInfo.pRasterizationState = &config_info.rasterizationInfo;
        pipelineInfo.pMultisampleState = &config_info.multisampleInfo;
        pipelineInfo.pColorBlendState = &config_info.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &config_info.depthStencilInfo;
        pipelineInfo.pDynamicState = &config_info.dynamicStateInfo;

        pipelineInfo.layout = config_info.pipelineLayout;
        pipelineInfo.renderPass = config_info.renderPass;
        pipelineInfo.subpass = config_info.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(VKDevice::Get().device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphiscPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline");
        }
    }

} // namespace VectorVertex
