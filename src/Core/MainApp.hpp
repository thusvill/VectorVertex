#pragma once

#include "../Render/vv_window.hpp"
#include "../Render/vv_device.hpp"
#include "../Render/vv_renderer.hpp"
#include "../Render/vv_buffer.hpp"
#include "../Render/vv_game_object.hpp"
#include "../Render/vv_camera.hpp"
#include "../Render/vv_descriptors.hpp"
#include "Keyboard_inputs.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <chrono>
#include <array>

namespace VectorVertex
{

    class VectorVetrex
    {
    public:
        VectorVetrex();
        ~VectorVetrex();

        VectorVetrex(const VectorVetrex &) = delete;
        VectorVetrex &operator=(const VectorVetrex &) = delete;

        static constexpr int WIDTH = 1980;
        static constexpr int HEIGHT = 1080;
        void run();

    private:
        void loadGameobjects();

        LveWindow lveWindow{WIDTH, HEIGHT, "VectorVertex"};
        LveDevice lveDevice{lveWindow};
        LveRenderer lveRenderer{lveWindow, lveDevice};

        std::unique_ptr<LveDescriptorPool> global_pool{};
        LveGameObject::Map gameObjects;
    };
} // namespace VectorVertex
