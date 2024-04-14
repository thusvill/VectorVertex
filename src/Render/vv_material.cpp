#include "vv_material.hpp"

namespace VectorVertex
{
    



VVMaterial VVMaterialLibrary::createMaterial(std::string name, MaterialData materialData)
{
    if(name.empty()){
        name = "material_" + std::to_string(m_Materials.size());
    }
   

    
    if (m_Materials.find(name) != m_Materials.end())
    {
        VV_CORE_WARN("Material already exists: {}", name);
        return getMaterial(name);
    }else{
    VVMaterial newMaterial{};
    newMaterial.m_ID = m_Materials.size();
    newMaterial.m_MaterialData = materialData;
    m_Materials[name] = newMaterial;
    VV_CORE_INFO("Created Material: {}", name);
    return newMaterial;
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

} // namespace VectorVertex