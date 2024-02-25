#include "MainApp.hpp"
#include "../Render/Render_Systems/lve_render_system.hpp"
#include "../Render/Render_Systems/lve_point_light_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
namespace lve
{
    VectorVetrex::VectorVetrex()
    {
        global_pool = LveDescriptorPool::Builder(lveDevice)
                          .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                          .build();
        loadGameobjects();
    }

    VectorVetrex::~VectorVetrex()
    {
    }

    void VectorVetrex::run()
    {
        std::vector<std::unique_ptr<LveBuffer>> ubo_buffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < ubo_buffers.size(); i++)
        {
            ubo_buffers[i] = std::make_unique<LveBuffer>(lveDevice, sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            ubo_buffers[i]->map();
        }

        auto global_set_layout = LveDescriptorSetLayout::Builder(lveDevice)
                                     .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                     .build();

        std::vector<VkDescriptorSet> global_descriptor_sets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < global_descriptor_sets.size(); i++)
        {
            auto buffer_info = ubo_buffers[i]->descriptorInfo();
            LveDescriptorWriter(*global_set_layout, *global_pool)
                .writeBuffer(0, &buffer_info)
                .build(global_descriptor_sets[i]);
        }

        LveRenderSystem renderSystem{lveDevice, lveRenderer.GetSwapchainRenderPass(), global_set_layout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{lveDevice, lveRenderer.GetSwapchainRenderPass(), global_set_layout->getDescriptorSetLayout()};
        LveCamera camera{};

        auto viewerObject = LveGameObject::CreateGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardInputs camControl{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!lveWindow.shouldClose())
        {
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            camControl.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            auto aspectRatio = lveRenderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1, 100.f);

            glfwPollEvents();
            if (auto commandBuffer = lveRenderer.BeginFrame())
            {
                int frame_index = lveRenderer.GetFrameIndex();
                FrameInfo frameInfo{
                    frame_index,
                    frameTime,
                    commandBuffer,
                    camera,
                    global_descriptor_sets[frame_index],
                    gameObjects};

                // update
                GlobalUBO ubo{};
                ubo.view = camera.GetView();
                ubo.projection = camera.GetProjection();
                ubo_buffers[frame_index]->writeToBuffer(&ubo);
                ubo_buffers[frame_index]->flush();
                // render
                lveRenderer.BeginSwapchainRenderPass(commandBuffer);
                renderSystem.renderGameobjects(frameInfo);
                pointLightSystem.render(frameInfo);
                lveRenderer.EndSwapchainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void VectorVetrex::loadGameobjects()
    {
        std::shared_ptr<LveModel> flat_vase = LveModel::createModelFromFile(lveDevice, "/home/bios/CLionProjects/2DEngine/Resources/Models/flat_vase.obj");
        auto flat_vase_object = LveGameObject::CreateGameObject();
        flat_vase_object.model = flat_vase;
        flat_vase_object.transform.translation = {-.5f, .5f, .0f};
        flat_vase_object.transform.scale = glm::vec3{3.f};
        gameObjects.emplace(flat_vase_object.getId(), std::move(flat_vase_object));

        std::shared_ptr<LveModel> smooth_vase = LveModel::createModelFromFile(lveDevice, "/home/bios/CLionProjects/2DEngine/Resources/Models/smooth_vase.obj");
        auto smooth_vase_object = LveGameObject::CreateGameObject();
        smooth_vase_object.model = smooth_vase;
        smooth_vase_object.transform.translation = {.5f, .5f, .0f};
        smooth_vase_object.transform.scale = glm::vec3{3.f};
        gameObjects.emplace(smooth_vase_object.getId(), std::move(smooth_vase_object));

        std::shared_ptr<LveModel> quad_model = LveModel::createModelFromFile(lveDevice, "/home/bios/CLionProjects/2DEngine/Resources/Models/quad.obj");
        auto quad = LveGameObject::CreateGameObject();
        quad.model = quad_model;
        quad.transform.translation = {.0f, .5f, .0f};
        quad.transform.scale = glm::vec3{3.f};
        gameObjects.emplace(quad.getId(), std::move(quad));
    }

} // namespace lve
