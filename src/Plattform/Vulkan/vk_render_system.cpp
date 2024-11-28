#include "vk_render_system.hpp"
#include <vk_api_data.hpp>
#include <Components.hpp>
#include <Entity.hpp>
#include <RenderCommand.hpp>
#include <GraphicsContext.hpp>
#include <vk_framebuffer.hpp>
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

        Ref<Shader> shader = Shader::CreateShader(std::filesystem::path(vertex_shader), std::filesystem::path(fragment_shader));
        pipeline = std::make_unique<VKPipeline>(pipelineConfig, *(VKShader *)shader->getAPIClass());
    }

    void VulkanRenderSystem::CreatePipeline(FrameBuffer &framebuffer, const std::string vertex_shader, const std::string fragment_shader)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfigInfo pipelineConfig{};
        VKPipeline::defaultPipelineConfigInfo(pipelineConfig);
        VKPipeline::enableAlphaBlending(pipelineConfig);
        for (int i = 1; i < framebuffer.GetSpecification().attachments.size(); i++)
        {
            VKPipeline::addAttachment(pipelineConfig, getVKFormat(framebuffer.GetSpecification().attachments[i]), false);
        }

        pipelineConfig.renderPass = reinterpret_cast<VKFrameBuffer *>(framebuffer.GetFrameBufferAPI())->getRenderpass();
        pipelineConfig.pipelineLayout = pipelineLayout;

        Ref<Shader> shader = Shader::CreateShader(std::filesystem::path(vertex_shader), std::filesystem::path(fragment_shader));

        pipeline = std::make_unique<VKPipeline>(pipelineConfig, *(VKShader*)shader->getAPIClass());
        VV_CORE_INFO("Pipeline Created with custom renderpass");
    }
}
