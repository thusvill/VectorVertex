#include "vv_material.hpp"

namespace VectorVertex
{

    std::unordered_map<std::string, VectorVertex::VVMaterial> VectorVertex::VVMaterialLibrary::m_Materials;

    void VVMaterialLibrary::InitMaterialLib()
    {
        //m_Materials["default"] = VVMaterial{0, MaterialData{glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}}};
        createMaterial("default", MaterialData{glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}});
        VV_CORE_INFO("Initialized Material Library");
    }

    uint32_t VVMaterialLibrary::getDefaultID()
    {
        return getMaterial("default").m_ID;
    }

    uint32_t VVMaterialLibrary::createMaterial(std::string name, MaterialData materialData)
    {
        if (name.empty())
        {
            name = "material_" + std::to_string(m_Materials.size());
        }

        if (m_Materials.find(name) != m_Materials.end())
        {
            VV_CORE_WARN("Material already exists: {}", name);
            return getMaterial(name).m_ID;
        }
        else
        {
            VVMaterial newMaterial{};
            newMaterial.m_ID = m_Materials.size();
            newMaterial.m_MaterialData = materialData;
            m_Materials[name] = newMaterial;
            VV_CORE_INFO("Created Material: {} ID: {}", name, newMaterial.m_ID);
            return newMaterial.m_ID;
        }
}

VVMaterial VVMaterialLibrary::getMaterial(uint32_t id)
{
    for (const auto &pair : m_Materials)
    {
        if (pair.second.m_ID == id)
        {
            return pair.second;
        }
    }
    VV_CORE_ERROR("Material not found: {}", id);
    return VVMaterial{};
}

VVMaterial VVMaterialLibrary::getMaterial(std::string name)
{
    return m_Materials[name];
}

void VVMaterialLibrary::updateMaterial(uint32_t id, MaterialData materialData)
{
    for (auto &pair : m_Materials)
    {
        if (pair.second.m_ID == id)
        {
            pair.second.m_MaterialData = materialData;
            return;
        }
    }
    VV_CORE_ERROR("Material not found: {}", id);
}

void VVMaterialLibrary::updateMaterial(std::string name, MaterialData materialData)
{
    m_Materials[name].m_MaterialData = materialData;
}

} // namespace VectorVertex