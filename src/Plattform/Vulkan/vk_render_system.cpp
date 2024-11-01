#include "vk_render_system.hpp"
#include <vk_api_data.hpp>
#include <Components.hpp>
#include <Entity.hpp>
#include <RenderCommand.hpp>
#include <GraphicsContext.hpp>
namespace VectorVertex
{


    void VulkanRenderSystem::CreatePipeline(const std::string vertex_shader, const std::string fragment_shader)
    {

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.renderPass = reinterpret_cast<VkRenderPass>(RenderCommand::GetRendererAPI()->GetRenderpass());
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VKPipeline>(pipelineConfig, vertex_shader, fragment_shader);
    }

    void VulkanRenderSystem::CreatePipeline(VkRenderPass renderpass, const std::string vertex_shader, const std::string fragment_shader)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.renderPass = renderpass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VKPipeline>(pipelineConfig, vertex_shader, fragment_shader);
    }
}