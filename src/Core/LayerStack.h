#ifndef GAMEENGINE_LAYERSTACK_H
#define GAMEENGINE_LAYERSTACK_H
#pragma once
#include "Layer.h"
#include "Base.h"
namespace VectorVertex {

    class LayerStack {
    public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);
        void UpdateAll(){
            for (size_t i = 0; i < m_Layers.size(); i++)
            {
                m_Layers[i]->OnUpdate();
            }
                }

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
        std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

        std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
        std::vector<Layer*>::const_iterator end()	const { return m_Layers.end(); }
        std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
        std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }
    private:
        std::vector<Layer*> m_Layers;
        unsigned int m_LayerInsertIndex = 0;
    };


} // VectorVertex

#endif //GAMEENGINE_LAYERSTACK_H