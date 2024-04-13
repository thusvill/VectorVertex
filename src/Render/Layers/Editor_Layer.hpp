#include <Layer.h>
#include "Imgui_Layer.hpp"
namespace VectorVertex
{

class EditorLayer: public Layer{
    public:
        EditorLayer();
        void SetupImgui(VVDevice* vv_device, VVRenderer* vv_renderer, VVWindow* vv_window);
        ~EditorLayer() = default;
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate() override;
        virtual void OnRender(FrameInfo &frameInfo) override;
        virtual void OnImGuiRender(FrameInfo &frameInfo) override;

        private:
            Imgui_Layer imgui_layer;
};
} 
