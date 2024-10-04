#pragma once
#include <Scene.hpp>
#include <Entity.hpp>
#include <functional>
#include <queue>

namespace VectorVertex
{
    extern std::queue<std::function<void()>> deferredActions;

    #define RUN_AFTER_FRAME(...) deferredActions.push([&]() { __VA_ARGS__; })

    extern void RunDeferredActions();
    

    class SceneHierarchy
    {
    public:
        SceneHierarchy() = default;
        SceneHierarchy(const Ref<Scene> &scene);

        void SetContext(const Ref<Scene> &scene);

        void OnImGuiRender();
        void ResetSelectedEntity(){
            m_SelectedEntity = {};
        }
        bool requestUpdateTextures;

    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
        Ref<Scene> m_Context;
        Entity m_SelectedEntity;
    };

} // namespace VectorVertex
