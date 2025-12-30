#include "Material.hpp"

namespace VectorVertex
{

    std::unordered_map<uint64_t, VectorVertex::Material> VectorVertex::MaterialLibrary::m_Materials;
    

    void MaterialLibrary::InitMaterialLib()
    {
        // m_Materials["default"] = Material{0, MaterialData{glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}}};
        
        VV_CORE_INFO("Initialized Material Library");
    }

    uint64_t MaterialLibrary::getDefaultID()
    {
        return createMaterial("default", MaterialData{glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}});;
    }

    uint64_t MaterialLibrary::createMaterial(std::string name, MaterialData materialData)
    {
        if (name.empty())
        {
            name = "material_" + std::to_string(m_Materials.size());
        }

        if (m_Materials.find(materialData.m_ID) != m_Materials.end())
        {
            VV_CORE_WARN("Material already exists: {}", name);
            return getMaterial(materialData.m_ID).m_MaterialData.m_ID;
        }
        else
        {
            Material newMaterial{};
            newMaterial.m_MaterialData = materialData;
            m_Materials[materialData.m_ID] = newMaterial;
            VV_CORE_INFO("Created Material: {} ID: {}", name, newMaterial.m_MaterialData.m_ID);
            return newMaterial.m_MaterialData.m_ID;
        }
    }

    uint64_t MaterialLibrary::createMaterialwithUUID(UUID id, std::string name, MaterialData materialData)
    {
        if (name.empty())
        {
            name = "material_" + std::to_string(m_Materials.size());
        }
        if (m_Materials.find(id) != m_Materials.end())
        {
            VV_CORE_WARN("Material already exists: {}", name);
            return getMaterial(id).m_MaterialData.m_ID;
        }
        else
        {
            Material newMaterial{};
            newMaterial.m_MaterialData = materialData;
            materialData.m_ID = id;
            m_Materials[id] = newMaterial;
            VV_CORE_INFO("Created Material: {} ID: {}", name, id);
            return id;
        }
    }

    Material MaterialLibrary::getMaterial(uint64_t id)
    {
        for (const auto &pair : m_Materials)
        {
            if (pair.second.m_MaterialData.m_ID == id)
            {
                return pair.second;
            }
        }
        VV_CORE_ERROR("Material not found: {}", id);
        Material _m{};
        _m.m_MaterialData.m_ID = -1;
        return _m;
    }

    bool MaterialLibrary::isMaterialAvailable(uint64_t id)
    {
        for (const auto &pair : m_Materials)
        {
            if (pair.second.m_MaterialData.m_ID == id)
            {
                return true;
            }
        }
        return false;
    }

    void MaterialLibrary::updateMaterial(uint64_t id, MaterialData materialData)
    {
        m_Materials[id].m_MaterialData = materialData;
    }

} // namespace VectorVertex