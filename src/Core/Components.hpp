#pragma once
#include <vv_texture.hpp>
#include <vv_model.hpp>
#include <vv_camera.hpp>
#include <glm/glm.hpp>
#include <vv_material.hpp>

namespace VectorVertex
{
    struct TextureComponent
    {
        TextureComponent(){
            m_ID = VVTextureLibrary::default_uuid;
            VV_CORE_INFO("Texture Created {}", m_ID);
        }
        TextureComponent(VVDevice &device,std::string name, std::string path){
            m_ID = VVTextureLibrary::Create(device, name, path);
            VV_CORE_INFO("Texture Created {}", m_ID);
        }
        uint64_t m_ID;
    };
    struct MaterialComponent{
        
        MaterialComponent(){
           m_ID = VVMaterialLibrary::getDefaultID();
        }
        MaterialComponent(std::string name, MaterialData materialData){
            m_ID = VVMaterialLibrary::createMaterial(name, materialData);
        }
        uint64_t m_ID;
    };
    struct IDComponent
    {
        IDComponent() = default;
        IDComponent(std::string _name): m_Name(_name){}
        std::string m_Name;
        UUID id;
    };

    struct TransformComponent
    {
        TransformComponent()=default;

        glm::vec3 translation{0.0f};
        glm::vec3 scale{1.f};
        glm::vec3 rotation{0.0f};
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };
    struct PointLightComponent
    {
        PointLightComponent()=default;
        PointLightComponent(glm::vec3 color, float intensity):color(color), light_intensity(intensity){}
        glm::vec3 color = glm::vec3{1.0f};
        float light_intensity = 1.0f;
    };
    struct MeshComponent
    {
        MeshComponent()=default;
        MeshComponent(VVDevice &device, const std::string &filepath){
            m_Model = VVModel::createModelFromFile(device, filepath);
        }
        Scope<VVModel> m_Model;
    };
    struct CameraComponent{
        CameraComponent()=default;
        VVCamera m_Camera;
    };
} // namespace VectorVertex
