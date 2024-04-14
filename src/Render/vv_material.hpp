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
            VVMaterialLibrary() = default;
            ~VVMaterialLibrary() = default;

            std::unordered_map<std::string, VVMaterial> m_Materials;

            VVMaterial createMaterial(std::string name, MaterialData materialData);
            VVMaterial getMaterial(uint32_t id);
            VVMaterial getMaterial(std::string name);
    };

} // namespace VectorVertex
