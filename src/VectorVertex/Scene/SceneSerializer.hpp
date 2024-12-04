#pragma once 
#include <Entity.hpp>
#include <Scene.hpp>

 namespace VectorVertex
 {
    class SceneSerializer{
        public:
        SceneSerializer(const Ref<Scene>& scene);
        void Serialize(const std::string& path);
        void SerializeRuntime(const std::string& path);

        bool Deserialize(const std::string& path);
        bool DeserializeRuntime(const std::string& path);

        Entity m_MainCamera;
        private:
        Ref<Scene> m_Scene;
    };
 } // namespace VectorVertex
 