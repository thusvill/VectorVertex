#include <Layer.h>
#include "Imgui_Layer.hpp"
namespace VectorVertex
{

class EditorLayer: public Layer{
    public:
        EditorLayer();
        void SetupImgui(VVDevice* vv_device, VVRenderer* vv_renderer, VVWindow* vv_window);
        ~EditorLayer() = default;
        void OnAttach();
        void OnDetach();
        void OnUpdate();
        void OnRender(VkCommandBuffer command_buffer);
        void OnImGuiRender(VkCommandBuffer command_buffer);

        private:
            Imgui_Layer imgui_layer;
};
} 
