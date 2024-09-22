#pragma once

#include <Log.h>
#include "vv_texture.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>

namespace VectorVertex
{
    struct MaterialPushConstant{
        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    };
    struct MaterialData
    {
        MaterialData() = default;
        MaterialData(glm::vec4 color):color(color){}
        MaterialPushConstant getPushData(){
            MaterialPushConstant push;
            push.color = color;
            return push;
        }

        UUID m_ID;
        std::string m_Name;
        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct VVMaterial
    {
        MaterialData m_MaterialData;
    };

    class VVMaterialLibrary{
        public:
            static void InitMaterialLib();
            static uint64_t getDefaultID();

            static std::unordered_map<uint64_t, VVMaterial> m_Materials;

            static uint64_t createMaterial(std::string name, MaterialData materialData);
            static VVMaterial getMaterial(uint64_t id);
            static bool isMaterialAvailable(uint64_t id);
            static uint64_t default_material;

        static void updateMaterial(uint64_t id, MaterialData materialData);
    };

} // namespace VectorVertex