#pragma once

#include "../Render/vv_window.hpp"
#include "../Render/vv_device.hpp"
#include "../Render/vv_renderer.hpp"
#include "../Render/vv_buffer.hpp"
#include "../Render/vv_game_object.hpp"
#include "../Render/vv_camera.hpp"
#include "../Render/vv_descriptors.hpp"
#include "../Render/vv_material.hpp"
//#include "../Render/vv_framebuffer.hpp"
#include "../Render/Layers/Editor_Layer.hpp"
#include "../Render/vv_texture.hpp"
#include "Keyboard_inputs.hpp"
#include "LayerStack.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <chrono>
#include <array>

namespace VectorVertex
{
    struct ProjectInfo
    {
        int width = 1980;
        int height = 1080;
        std::string title = "VectorVertex";
    };


        class VectorVetrex
    {
    public:
        VectorVetrex(ProjectInfo &info);
        ~VectorVetrex();

        VectorVetrex(const VectorVetrex &) = delete;
        VectorVetrex &operator=(const VectorVetrex &) = delete;

        GLFWwindow *GetNativeWindow() { return vvWindow.getGLFWwindow(); }
        VVDevice &GetDevice() { return vvDevice; }

        int WIDTH, HEIGHT;
        std::string project_name;
        void run();

    private:
        void loadGameobjects();

        VVWindow vvWindow{WIDTH, HEIGHT, project_name};
        VVDevice vvDevice{vvWindow};
        VVRenderer renderer{vvWindow, vvDevice};    

        EditorLayer* editor_layer;


        LayerStack layers{};
        //VVMaterialLibrary materials{};
        Scope<VVDescriptorPool> global_pool{};
        VVGameObject::Map gameObjects;

        
    };
} // namespace VectorVertex
