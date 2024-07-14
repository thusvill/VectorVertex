//
// Created by bios on 9/14/23.
//

#ifndef GAMEENGINE_LAYER_H
#define GAMEENGINE_LAYER_H

#pragma once
#include "Base.h"
#include "../Render/vv_frame_info.hpp"

namespace VectorVertex {
    class Layer {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach(){}
        virtual void OnDetach(){}
        virtual void OnUpdate(){}
        virtual void OnRender(FrameInfo &frameInfo) {}
        virtual void OnImGuiRender(FrameInfo &frameInfo) {}
        const std::string& GetName() const {return m_DebugName;}

    private:
        std::string m_DebugName;
    };
}


#endif //GAMEENGINE_LAYER_H
