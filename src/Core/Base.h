//
// Created by bios on 9/12/23.
//

#ifndef GAMEENGINE_BASE_H
#define GAMEENGINE_BASE_H

#pragma once
#include "memory"
#include <string>

#define VV_EXPAND_MACRO(x) x
#define VV_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define VV_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define VV_DEBUG true

#ifdef VV_DEBUG
#define VV_ENABLE_ASSERTS
#endif



namespace VectorVertex{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


    #define CONVERT_REF(type, ptr) std::shared_ptr<type>(ptr)
}
#include "Log.h"
#include "Assert.h"

#endif //GAMEENGINE_BASE_H
