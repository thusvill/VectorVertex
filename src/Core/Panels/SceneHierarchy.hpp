#pragma once
#include <Scene.hpp>

namespace VectorVertex
{
    class SceneHierarchy
    {
        public:
        SceneHierarchy()=default;
        SceneHierarchy(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);

        void OnImGuiRender();

        private:
        void DrawEntityNode(Entity entity);
        Ref<Scene> m_Context;
        UUID m_SelectedEntityID;
    };

} // namespace VectorVertex
