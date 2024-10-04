#include "SceneSerializer.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Entity.hpp>
#include <SceneHierarchy.hpp>
#include <vv_uuid.hpp>
#include <vv_camera.hpp>

namespace YAML
{
    template <>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2 &rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node &node, glm::vec2 &rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3 &rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node &node, glm::vec3 &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4 &rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node &node, glm::vec4 &rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template <>
    struct convert<VectorVertex::UUID>
    {
        static Node encode(const VectorVertex::UUID &uuid)
        {
            Node node;
            node.push_back((uint64_t)uuid);
            return node;
        }

        static bool decode(const Node &node, VectorVertex::UUID &uuid)
        {
            uuid = node.as<uint64_t>();
            return true;
        }
    };
}
namespace VectorVertex
{
    YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec2 &v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    static void SerializeEntity(YAML::Emitter &out, Entity entity)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << entity.GetComponent<IDComponent>().id;

        if (entity.HasComponent<IDComponent>())
        {
            out << YAML::Key << "IDComponent";
            out << YAML::BeginMap;

            auto &idC = entity.GetComponent<IDComponent>();
            out << YAML::Key << "UUID" << YAML::Value << idC.id;
            out << YAML::Key << "Name" << YAML::Value << idC.m_Name;
            out << YAML::EndMap;
        }
        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;

            auto &tC = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Position" << YAML::Value << tC.translation;
            out << YAML::Key << "Rotation" << YAML::Value << tC.rotation;
            out << YAML::Key << "Scale" << YAML::Value << tC.scale;
            out << YAML::EndMap;
        }
        if (entity.HasComponent<MaterialComponent>())
        {
            out << YAML::Key << "MaterialComponent";
            out << YAML::BeginMap;

            auto mC = VVMaterialLibrary::getMaterial(entity.GetComponent<MaterialComponent>().m_ID).m_MaterialData;
            out << YAML::Key << "m_ID" << YAML::Value << mC.m_ID;
            out << YAML::Key << "m_Name" << YAML::Value << mC.m_Name;
            out << YAML::Key << "color" << YAML::Value << mC.color;
            out << YAML::EndMap;
        }
        if (entity.HasComponent<TextureComponent>())
        {
            out << YAML::Key << "TextureComponent";
            out << YAML::BeginMap;

            auto tC = VVTextureLibrary::GetTexture(entity.GetComponent<TextureComponent>().m_ID).data;
            out << YAML::Key << "m_ID" << YAML::Value << tC.m_ID;
            out << YAML::Key << "m_Name" << YAML::Value << tC.m_Name;
            out << YAML::Key << "m_path" << YAML::Value << tC.m_path;
            out << YAML::EndMap;
        }
        if (entity.HasComponent<PointLightComponent>())
        {
            out << YAML::Key << "PointLightComponent";
            out << YAML::BeginMap;

            auto pC = entity.GetComponent<PointLightComponent>();
            out << YAML::Key << "color" << YAML::Value << pC.color;
            out << YAML::Key << "light_intensity" << YAML::Value << pC.light_intensity;
            out << YAML::Key << "radius" << YAML::Value << pC.radius;
            out << YAML::EndMap;
        }
        if (entity.HasComponent<MeshComponent>())
        {
            out << YAML::Key << "MeshComponent";
            out << YAML::BeginMap;

            auto &mC = entity.GetComponent<MeshComponent>();
            out << YAML::Key << "m_ID" << YAML::Value << mC.m_ID;
            out << YAML::Key << "path" << YAML::Value << mC.path;
            out << YAML::EndMap;
        }
        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;

            auto &cC = entity.GetComponent<CameraComponent>().m_Camera.GetCameraData();
            out << YAML::Key << "m_ProjectionType" << YAML::Value << (int)cC.m_ProjectionType;
            out << YAML::Key << "fov" << YAML::Value << cC.fov;
            out << YAML::Key << "aspect" << YAML::Value << cC.aspect;
            out << YAML::Key << "near" << YAML::Value << cC.near;
            out << YAML::Key << "far" << YAML::Value << cC.far;
            out << YAML::Key << "orthoWidth" << YAML::Value << cC.orthoWidth;
            out << YAML::Key << "orthoHeight" << YAML::Value << cC.orthoHeight;
            out << YAML::Key << "orthoNear" << YAML::Value << cC.orthoNear;
            out << YAML::Key << "orthoFar" << YAML::Value << cC.orthoFar;
            out << YAML::Key << "mainCamera" << YAML::Value << entity.GetComponent<CameraComponent>().mainCamera;
            out << YAML::EndMap;
        }

        out << YAML::EndMap;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene> &scene)
        : m_Scene(scene)
    {
    }

    bool SceneSerializer::DeserializeRuntime(const std::string &path)
    {
        VV_CORE_ASSERT(false);
        return false;
    }

    bool SceneSerializer::Deserialize(const std::string &path)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path);
        }
        catch (YAML::ParserException e)
        {
            VV_CORE_ERROR("Failed to load .hazel file '{0}'\n     {1}", path, e.what());
            return false;
        }

        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        VV_CORE_TRACE("Deserializing scene '{0}'", sceneName);
        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {

                if (auto ic = entity["IDComponent"])
                {
                    Entity deserialized_entity = m_Scene->CreateEntityWithUUID(ic["UUID"].as<UUID>(), ic["Name"].as<std::string>());
                    if (auto tComponent = entity["TransformComponent"])
                    {
                        auto &tc = deserialized_entity.GetORCreateComponent<TransformComponent>();
                        tc.translation = tComponent["Position"].as<glm::vec3>();
                        tc.rotation = tComponent["Rotation"].as<glm::vec3>();
                        tc.scale = tComponent["Scale"].as<glm::vec3>();
                    }

                    if (entity["MaterialComponent"])
                    {

                        if (auto mComponent = entity["MaterialComponent"])
                        {
                            auto &mc = deserialized_entity.GetORCreateComponent<MaterialComponent>();
                            MaterialData _data;
                            _data.color = mComponent["color"].as<glm::vec4>();
                            _data.m_ID = mComponent["m_ID"].as<UUID>();
                            _data.m_Name = mComponent["m_Name"].as<std::string>();
                            mc.m_ID = VVMaterialLibrary::createMaterialwithUUID(mComponent["m_ID"].as<UUID>(),mComponent["m_Name"].as<std::string>(), _data);
                        }
                    }
                    if (entity["TextureComponent"])
                    {

                        if (auto tComponent = entity["TextureComponent"])
                        {
                            auto &tc = deserialized_entity.GetORCreateComponent<TextureComponent>();
                            tc.m_ID = tComponent["m_ID"].as<UUID>();
                            VVTextureLibrary::CreateWithUUID(tComponent["m_ID"].as<UUID>(), tComponent["m_Name"].as<std::string>(), tComponent["m_path"].as<std::string>());
                            VVTextureLibrary::UpdateDescriptors();
                        }
                    }
                    if (entity["PointLightComponent"])
                    {

                        if (auto pComponent = entity["PointLightComponent"])
                        {
                            auto &pc = deserialized_entity.GetORCreateComponent<PointLightComponent>();
                            pc.color = pComponent["color"].as<glm::vec3>();
                            pc.radius = pComponent["radius"].as<float>();
                            pc.light_intensity = pComponent["light_intensity"].as<float>();
                        
                        }
                    }
                    if (entity["MeshComponent"])
                    {

                        if (auto mComponent = entity["MeshComponent"])
                        {
                            auto &mc = deserialized_entity.GetORCreateComponent<MeshComponent>();
                            mc.m_ID = mComponent["m_ID"].as<UUID>();
                            mc.path = mComponent["path"].as<std::string>();
                            mc.m_Model = VVModel::createModelFromFile(mc.path);
                        
                        }
                    }
                    if (entity["CameraComponent"])
                    {

                        if (auto cComponent = entity["CameraComponent"])
                        {
                            auto &cc = deserialized_entity.GetORCreateComponent<CameraComponent>();
                            VVCamera::CameraData data;
                            data.m_ProjectionType = (VVCamera::ProjectionType)cComponent["m_ProjectionType"].as<int>();
                            data.fov = cComponent["fov"].as<float>();
                            data.aspect = cComponent["aspect"].as<float>();
                            data.near = cComponent["near"].as<float>();
                            data.far = cComponent["far"].as<float>();
                            data.orthoWidth = cComponent["orthoWidth"].as<float>();
                            data.orthoHeight = cComponent["orthoHeight"].as<float>();
                            data.orthoNear = cComponent["orthoNear"].as<float>();
                            data.orthoFar = cComponent["orthoFar"].as<float>();
                            cc.m_Camera.SetCameraData(data);
                            cc.mainCamera = cComponent["mainCamera"].as<bool>();
                            //m_MainCamera = deserialized_entity;
                            if(cc.mainCamera)
                                m_Scene->SetMainCamera(&deserialized_entity);
                            
                        }
                    }
                }
            }
            
            VVTextureLibrary::UpdateDescriptors();
        }

        return true;
    }

    void SceneSerializer::SerializeRuntime(const std::string &path)
    {
        VV_CORE_ASSERT(false);
    }

    void SceneSerializer::Serialize(const std::string &path)
    {
        if(path.empty()){
            return;
        }
        {
            std::filesystem::path dirPath = std::filesystem::path(path).parent_path();

            if (!std::filesystem::exists(dirPath))
            {
                std::filesystem::create_directories(dirPath);
            }
        }

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Name;
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        for (auto &id : m_Scene->m_Entities)
        {
            Entity entity = id.second;
            if (!entity)
                return;

            SerializeEntity(out, entity);
        };

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
    }

} // namespace VectorVertex
