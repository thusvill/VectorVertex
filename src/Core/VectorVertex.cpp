#include "VectorVertex.hpp"
#include "Log.h"
#include "../Render/Render_Systems/vv_point_light_system.hpp"
#include "../Render/Render_Systems/vv_render_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>

namespace VectorVertex
{
    VectorVetrex::VectorVetrex(ProjectInfo &info) : WIDTH(info.width), HEIGHT(info.height), project_name(info.title)
    {
        VV_CORE_WARN("Application is Started!");
        VV_CORE_WARN("Initializing ...");

        global_pool = VVDescriptorPool::Builder(vvDevice)
                          .setMaxSets(VVSwapChain::MAX_FRAMES_IN_FLIGHT)
                          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VVSwapChain::MAX_FRAMES_IN_FLIGHT)
                          .build();
        

        VkInstance vulkanInstance;

        vulkanInstance = vvDevice.getInstance();

        

        VV_CORE_ASSERT(vulkanInstance, "Vulkan Instance should not be null!");

        ImguiConfig imguiConfig;
        imguiConfig.instance = vulkanInstance; // Assign Vulkan instance handle
        imguiConfig.Device = vvDevice.device();
        imguiConfig.renderer = &renderer;
        imguiConfig.renderPass = renderer.GetSwapchainRenderPass();
        imguiConfig.PhysicalDevice = vvDevice.getPhysicalDevice();
        imguiConfig.graphicsQueue = vvDevice.graphicsQueue();
        imguiConfig.imageCount = static_cast<uint32_t>(renderer.GetSwapchainImageCount());

        // Initialize ImGui layer
        imgui_layer.InitializeImgui(imguiConfig, vvWindow.getGLFWwindow());
        layers.PushLayer(&imgui_layer);

        VV_CORE_WARN("Initialized!");


        //TODO: implement Asset system
        VV_CORE_WARN("Loading Gameobjects ...");
        loadGameobjects();
        VV_CORE_WARN("Gameobjects Loaded!");
    }

    VectorVetrex::~VectorVetrex()
    {
    }

    void VectorVetrex::run()
    {
        
        std::vector<std::unique_ptr<VVBuffer>> ubo_buffers(VVSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < ubo_buffers.size(); i++)
        {
            ubo_buffers[i] = std::make_unique<VVBuffer>(vvDevice, sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            ubo_buffers[i]->map();
        }

        auto global_set_layout = LveDescriptorSetLayout::Builder(vvDevice)
                                     .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                     .build();

        std::vector<VkDescriptorSet> global_descriptor_sets(VVSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < global_descriptor_sets.size(); i++)
        {
            auto buffer_info = ubo_buffers[i]->descriptorInfo();
            LveDescriptorWriter(*global_set_layout, *global_pool)
                .writeBuffer(0, &buffer_info)
                .build(global_descriptor_sets[i]);
        }

        LveRenderSystem renderSystem{vvDevice, renderer.GetSwapchainRenderPass(), global_set_layout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{vvDevice, renderer.GetSwapchainRenderPass(), global_set_layout->getDescriptorSetLayout()};
        VVCamera camera{};

        auto viewerObject = VVGameObject::CreateGameObject();
        viewerObject.transform.translation = glm::vec3(-1.48, -0.77, -2.17);
        viewerObject.transform.rotation = glm::vec3(-0.41, 0.87, 0.0f);
        KeyboardInputs camControl{};

        auto currentTime = std::chrono::high_resolution_clock::now();



        while (!vvWindow.shouldClose())
        {

            // std::cout << "Camera | position x:" << viewerObject.transform.translation.x << " y:" << viewerObject.transform.translation.y << " z:" << viewerObject.transform.translation.z << "\n Camera| rotation x:" << viewerObject.transform.rotation.x << " y:" << viewerObject.transform.rotation.y << " z:" << viewerObject.transform.rotation.z << std::endl;
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            camControl.moveInPlaneXZ(vvWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            auto aspectRatio = renderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1, 100.f);

            glfwPollEvents();
            if (auto commandBuffer = renderer.BeginFrame())
            {
                int frame_index = renderer.GetFrameIndex();
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
                ubo.inverse_view_matrix = camera.GetInverseViewMatrix();
                pointLightSystem.Update(frameInfo, ubo);
                ubo_buffers[frame_index]->writeToBuffer(&ubo);
                ubo_buffers[frame_index]->flush();
                // render
                renderer.BeginSwapchainRenderPass(commandBuffer);

                imgui_layer.Begin();

                ImGui::ShowDemoWindow();

                imgui_layer.End(commandBuffer);

                //renderSystem.renderImGui(commandBuffer);

                renderSystem.renderGameobjects(frameInfo);

                pointLightSystem.render(frameInfo);
                renderer.EndSwapchainRenderPass(commandBuffer);
                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(vvDevice.device());
    }

    void VectorVetrex::loadGameobjects()
    {
        std::shared_ptr<VVModel> VVModel = nullptr;

        // VVModel = VVModel::createModelFromFile(vvDevice, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/Rubik.fbx");
        // auto supra5_object = VVGameObject::CreateGameObject();
        // supra5_object.model = VVModel;
        // supra5_object.color = {.1f, .0f, .0f};
        // supra5_object.transform.translation = {1.0f, .0f, .0f};
        // supra5_object.transform.scale = glm::vec3{0.1f};
        // // supra_object.transform.rotation.z = 1 * 3.15;
        // gameObjects.emplace(supra5_object.getId(), std::move(supra5_object));

        // VVModel = VVModel::createModelFromFile(vvDevice, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/supra/supra_a80.obj");
        // auto supra_object = VVGameObject::CreateGameObject();
        // supra_object.model = VVModel;
        // supra_object.color = {.1f, .0f, .0f};
        // supra_object.transform.translation = {.5f, .5f, .0f};
        // supra_object.transform.scale = glm::vec3{0.2f};
        // // supra_object.transform.rotation.z = 1 * 3.15;
        // gameObjects.emplace(supra_object.getId(), std::move(supra_object));

        VVModel = VVModel::createModelFromFile(vvDevice, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/quad.obj");
        auto quad = VVGameObject::CreateGameObject();
        quad.model = VVModel;
        quad.transform.translation = {.0f, .5f, .0f};
        quad.transform.scale = glm::vec3{3.f};
        gameObjects.emplace(quad.getId(), std::move(quad));

        {
            std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f} //
            };

            for (int i = 0; i < lightColors.size(); i++)
            {
                auto pointLight = VVGameObject::MakePointLight(0.5f);
                pointLight.color = lightColors[i];
                auto rotateLight = glm::rotate(
                    glm::mat4(1.f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.f, -1.f, 0.f});
                pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
                gameObjects.emplace(pointLight.getId(), std::move(pointLight));
            }
        }
    }

} // namespace VectorVertex
