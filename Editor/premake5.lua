project "VVEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetdir "%{wks.location}/bin/%{prj.name}/%{cfg.buildcfg}"
    objdir "%{wks.location}/bin-int/%{prj.name}/%{cfg.buildcfg}"

    files {
        "src/**.hpp",
        "src/**.cpp",
        "src/**.h",
    }

    includedirs {
        "%{wks.location}/VectorVertex/vendor/spdlog/include",
        "%{wks.location}/VectorVertex/src",
        "%{wks.location}/VectorVertex/vendor",
        "%{wks.location}/VectorVertex/vendor/fbx/include",
        "%{wks.location}/VectorVertex/vendor/imgui",
    }

    links {
        "VectorVertex",
        "fmt",
    }
    
    defines { "STBI_NO_SIMD" }

    filter "system:linux"
        buildoptions { "-mno-sse2" }



    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
