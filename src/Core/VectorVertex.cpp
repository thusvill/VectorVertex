#include "VectorVertex.hpp"
#include "Log.h"
#include "../Render/Render_Systems/vv_point_light_system.hpp"
#include "../Render/Render_Systems/vv_render_system.hpp"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <iostream>

namespace VectorVertex
{

    VkImageView CreateColorAttachmentImageView(VkDevice device, VkImage image, VkFormat format);
    VkImage CreateImage(VVDevice &vvdevice, VkImageUsageFlags usage, uint32_t width, uint32_t height);
    void TransitionImageLayout(VVDevice *vvdevice, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool hasStencilComponent(VkFormat format);

    VectorVetrex::VectorVetrex(ProjectInfo &info) : WIDTH(info.width), HEIGHT(info.height), project_name(info.title)
    {

        VV_CORE_WARN("Application is Started!");
        VV_CORE_WARN("Initializing ...");

        editor_layer = new EditorLayer(vvDevice, vvWindow, renderer);

        editor_layer->SetupImgui(&vvDevice, &renderer, &vvWindow);
        layers.PushLayer(editor_layer);

        editor_layer->OnAttach();

        VV_CORE_WARN("Initialized!");
    }

    VectorVetrex::~VectorVetrex()
    {
    }

    void VectorVetrex::run()
    {

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!vvWindow.shouldClose())
        {
            layers.UpdateAll();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            glfwPollEvents();
            // {
            //     for (auto &kv : gameObjects)
            //     {
            //         auto &obj = kv.second;
            //         if (!renderer.Get_Swapchain().isWaitingForFence)
            //         {
            //             VV_CORE_TRACE("Fence Done!");
            //             // update textures
            //             auto imageInfo = VVTextureLibrary::GetTexture(obj.texture.data.m_ID).getDescriptorImageInfo();
            //             VVDescriptorWriter(*VVTextureLibrary::textureImageDescriptorLayout, *VVTextureLibrary::texture_pool)
            //                 .writeImage(0, &imageInfo)
            //                 .build(obj.texture.data.m_descriptorSet);
            //         }
            //         else
            //         {
            //             VV_CORE_TRACE("Waiting For Fence");
            //         }
            //     }
            // }

            if (auto commandBuffer = renderer.BeginFrame())
            {

                int frame_index = renderer.GetFrameIndex();
                std::unordered_map<int, VkDescriptorSet> descriptor_sets;
                FrameInfo frameInfo{
                    frame_index,
                    frameTime,
                    commandBuffer,
                    renderer};

                editor_layer->OnRender(frameInfo);
                // Main Window Renderer
                {
                    renderer.BeginSwapchainRenderPass(commandBuffer);
                    editor_layer->OnImGuiRender(frameInfo);
                    renderer.EndSwapchainRenderPass(commandBuffer);
                }

                renderer.EndFrame();
            }
            
        }

        vkDeviceWaitIdle(vvDevice.device());
    }

    // {
    //     void VectorVetrex::loadGameobjects()
    //     {

    //         std::shared_ptr<VVModel> VVModel = nullptr;

    //         // VVModel = VVModel::createModelFromFile(vvDevice, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/Rubik.fbx");
    //         // auto supra5_object = VVGameObject::CreateGameObject();
    //         // supra5_object.model = VVModel;
    //         // supra5_object.color = {.1f, .0f, .0f};
    //         // supra5_object.transform.translation = {1.0f, .0f, .0f};
    //         // supra5_object.transform.scale = glm::vec3{0.1f};
    //         // // supra_object.transform.rotation.z = 1 * 3.15;
    //         // gameObjects.emplace(supra5_object.getId(), std::move(supra5_object));

    //         uint64_t dirt_texture = VVTextureLibrary::Create(vvDevice, "DirtAlbedo", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Textures/DirtAlbedo.png");
    //         uint64_t grid_texture = VVTextureLibrary::Create(vvDevice, "Grid", "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Textures/BackgroundGreyGridSprite.png");

    //         VVModel = VVModel::createModelFromFile(vvDevice, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/S15/source/S15_Nobonnet.obj");
    //         auto supra_object = VVGameObject::CreateGameObject("S15 Model");
    //         supra_object.model = VVModel;
    //         supra_object.color = {.1f, .0f, .0f};
    //         supra_object.transform.translation = {.5f, 0.25f, .0f};
    //         supra_object.transform.rotation = glm::vec3(3.15f, 0.0f, 0.0f);
    //         supra_object.transform.scale = glm::vec3{0.45f};
    //         supra_object.material_id = VVMaterialLibrary::createMaterial("supra_body", MaterialData(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
    //         supra_object.texture.data = VVTextureLibrary::GetTexture(grid_texture).data;
    //         // supra_object.transform.rotation.z = 1 * 3.15;

    //         gameObjects.emplace(supra_object.getId(), std::move(supra_object));

    //         VVModel = VVModel::createModelFromFile(vvDevice, "/home/bios/CLionProjects/VectorVertex/3DEngine/Resources/Models/quad.obj");
    //         auto quad = VVGameObject::CreateGameObject("Plane");
    //         quad.model = VVModel;
    //         quad.material_id = VVMaterialLibrary::createMaterial("ground", MaterialData(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
    //         quad.transform.translation = {.0f, .5f, .0f};
    //         quad.transform.rotation = glm::vec3(0.0f);
    //         // quad.transform.rotation.y = 1 * (3.15 / 2);
    //         quad.transform.scale = glm::vec3{3.f};
    //         quad.texture.data = VVTextureLibrary::GetTexture(dirt_texture).data;
    //         gameObjects.emplace(quad.getId(), std::move(quad));

    //         {
    //             std::vector<glm::vec3> lightColors{
    //                 {1.f, .1f, .1f},
    //                 {.1f, .1f, 1.f},
    //                 {.1f, 1.f, .1f},
    //                 {1.f, 1.f, .1f},
    //                 {.1f, 1.f, 1.f},
    //                 {1.f, 1.f, 1.f} //
    //             };

    //             for (int i = 0; i < lightColors.size(); i++)
    //             {
    //                 auto pointLight = VVGameObject::MakePointLight(0.5f);
    //                 pointLight.color = lightColors[i];
    //                 auto rotateLight = glm::rotate(
    //                     glm::mat4(1.f),
    //                     (i * glm::two_pi<float>()) / lightColors.size(),
    //                     {0.f, -1.f, 0.f});
    //                 pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
    //                 gameObjects.emplace(pointLight.getId(), std::move(pointLight));
    //             }
    //         }
    //     }
    // }

} // namespace VectorVertex
