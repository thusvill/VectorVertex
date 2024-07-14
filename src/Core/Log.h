//
// Created by bios on 9/13/23.
//


#pragma once
#include <spdlog/spdlog.h>
#include <Base.h>
#include <Assert.h>
namespace VectorVertex {

    class Log {
    public:
        static void Init();

        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };
}

#if defined(VV_DEBUG)
// Core log macros
#define VV_CORE_TRACE(...)    ::VectorVertex::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VV_CORE_INFO(...)     ::VectorVertex::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VV_CORE_WARN(...)     ::VectorVertex::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VV_CORE_ERROR(...)    ::VectorVertex::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VV_CORE_CRITICAL(...) ::VectorVertex::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define VV_TRACE(...)    ::VectorVertex::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VV_INFO(...)     ::VectorVertex::Log::GetClientLogger()->info(__VA_ARGS__)
#define VV_WARN(...)     ::VectorVertex::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VV_ERROR(...)    ::VectorVertex::Log::GetClientLogger()->error(__VA_ARGS__)
#define VV_CRITICAL(...) ::VectorVertex::Log::GetClientLogger()->critical(__VA_ARGS__)
#else
#define VV_CORE_TRACE(...)
#define VV_CORE_INFO(...)
#define VV_CORE_WARN(...)
#define VV_CORE_ERROR(...)
#define VV_CORE_CRITICAL(...)

// Client log macros
#define VV_TRACE(...)
#define VV_INFO(...)
#define VV_WARN(...)
#define VV_ERROR(...)
#define VV_CRITICAL(...)
#endif

