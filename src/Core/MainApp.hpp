#pragma once

#include "../Render/lve_window.hpp"
#include "../Render/lve_device.hpp"
#include "../Render/lve_renderer.hpp"
#include "../Render/lve_buffer.hpp"
#include "../Render/lve_game_object.hpp"
#include "../Render/lve_camera.hpp"
#include "../Render/lve_descriptors.hpp"
#include "Keyboard_inputs.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <chrono>
#include <array>

namespace lve
{
    struct GlobalUBO
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::vec4 ambient_color{1.f, 1.f, 1.f, .02f};
        glm::vec3 light_position{-1.f};
        alignas(16) glm::vec4 light_color{1.f};
    };

    class VectorVetrex
    {
    public:
        VectorVetrex();
        ~VectorVetrex();

        VectorVetrex(const VectorVetrex &) = delete;
        VectorVetrex &operator=(const VectorVetrex &) = delete;

        static constexpr int WIDTH = 600;
        static constexpr int HEIGHT = 800;
        void run();

    private:
        void loadGameobjects();

        LveWindow lveWindow{WIDTH, HEIGHT, "VectorVertex"};
        LveDevice lveDevice{lveWindow};
        LveRenderer lveRenderer{lveWindow, lveDevice};

        std::unique_ptr<LveDescriptorPool> global_pool{};
        LveGameObject::Map gameObjects;
    };
} // namespace lve
