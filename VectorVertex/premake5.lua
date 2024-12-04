project "VectorVertex"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    targetdir "%{wks.location}/bin/%{prj.name}/%{cfg.buildcfg}"
    objdir "%{wks.location}/bin-int/%{prj.name}/%{cfg.buildcfg}"

    pchheader "src/vvpch.hpp"
    pchsource "src/vvpch.cpp"


    includedirs {
        "src",
        "src/Core",
        "src/Core/Math",
        "src/Core/Utils",
        "src/Plattform",
        "src/Plattform/Vulkan",
        "src/Plattform/Vulkan/Render_Systems",
        "src/Render",
        "src/VectorVertex",
        "src/VectorVertex/Events",
        "src/VectorVertex/Layers",
        "src/VectorVertex/Panels",
        "src/VectorVertex/Scene",
        "vendor",
        "vendor/entt",
        "vendor/imgui",
        "vendor/SPIRV-Cross/include",
        "vendor/imgui/backends/vulkan",
        "vendor/ImGuizmo",
        "vendor/yaml-cpp/include",
        "vendor/shaderc/include",
        "vendor/fbx/include",

    }

    files {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        
    }


    links {
        "glfw",
        "fmt",
        "yaml-cpp",
        "glslang",
        "shaderc",
        "shaderc_combined",
        "SPIRV-Tools",
        "vulkan",
    }

    defines {
        "BUILD_DLL"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        buildoptions "-w -g"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
