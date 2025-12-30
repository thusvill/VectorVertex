//
// Created by bios on 10/15/23.
//

#ifndef GAMEENGINE_ASSERT_H
#define GAMEENGINE_ASSERT_H
#pragma once
#include "Base.h"
#include <filesystem>

#if defined(_MSC_VER)
#define VV_DEBUGBREAK() __debugbreak()
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#define VV_DEBUGBREAK() __builtin_trap()
#else
#include <signal.h>
    #if defined(SIGTRAP)
        #define VV_DEBUGBREAK() raise(SIGTRAP)
    #else
        #define VV_DEBUGBREAK() ((void)0)
    #endif
#endif


#ifdef VV_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define VV_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { VV##type##ERROR(msg, __VA_ARGS__); VV_DEBUGBREAK(); } }
	#define VV_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VV_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define VV_INTERNAL_ASSERT_NO_MSG(type, check) VV_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VV_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define VV_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define VV_INTERNAL_ASSERT_GET_MACRO(...) VV_EXPAND_MACRO( VV_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VV_INTERNAL_ASSERT_WITH_MSG, VV_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define VV_ASSERT(...) VV_EXPAND_MACRO( VV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define VV_CORE_ASSERT(...) VV_EXPAND_MACRO( VV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define VV_ASSERT(...)
#define VV_CORE_ASSERT(...)
#endif

#endif //GAMEENGINE_ASSERT_H
