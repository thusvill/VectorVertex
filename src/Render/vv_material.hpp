#pragma once

#include <Log.h>

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

namespace VectorVertex
{
    struct MaterialData
    {
        MaterialData() = default;
        MaterialData(glm::vec4 color):color(color){}

        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct VVMaterial
    {
        uint32_t m_ID;
        MaterialData m_MaterialData;
    };

    class VVMaterialLibrary{
        public:
            static void InitMaterialLib();
            static uint32_t getDefaultID();

            static std::unordered_map<std::string, VVMaterial> m_Materials;

            static uint32_t createMaterial(std::string name, MaterialData materialData);
            static VVMaterial getMaterial(uint32_t id);
            static VVMaterial getMaterial(std::string name);

        static void updateMaterial(uint32_t id, MaterialData materialData);
        static void updateMaterial(std::string name, MaterialData materialData);
    };

} // namespace VectorVertex