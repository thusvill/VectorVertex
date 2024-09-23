cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)
project(VectorVertex)

# Add GLFW library
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

# Add Vulkan library
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# Set the path to the FBX SDK
set(FBX_SDK_DIR "src/Render/Model_Loaders/fbx")

# Add include directories
include_directories("src/Render/Model_Loaders")
include_directories("src/Render/Render_Systems")
include_directories("src/Render/vendor")
include_directories("src/Core/vendor")
include_directories("src/Core/Panels") 
include_directories("src/Core/Scene")    
include_directories("src/Render/Layers")
include_directories("src/Render/vendor/imgui")
include_directories("src/vendor/entt")
include_directories(${FBX_SDK_DIR}/include)

file(GLOB IMGUI_SOURCES "src/Render/vendor/imgui/*.cpp")

# Collect all .cpp, .h, and .hpp files in the source directory
file(GLOB_RECURSE SOURCES ${IMGUI_SOURCES} "src/Render/**.cpp" "src/Core/Scene/**.cpp" "src/Core/Scene/**.hpp" "src/Core/Panels/**.cpp" "src/Core/Panels/**.hpp"  "src/Core/**.cpp" "src/Render/Layers/*.cpp" "src/Render/Layers/Editor_Layer.cpp" "src/Render/Layers/Editor_Layer.hpp" "src/Render/Layers/Imgui_Layer.cpp" "src/Render/Layers/Imgui_Layer.hpp" )

# Add executable
add_executable(VectorVertex main.cpp ${SOURCES})

# Link libraries
target_link_libraries(VectorVertex glfw fmt ${Vulkan_LIBRARIES})

# Link the FBX SDK library
find_library(FBX_LIBRARY fbxsdk HINTS ${FBX_SDK_DIR}/lib/debug)
target_link_libraries(VectorVertex ${FBX_LIBRARY})

# Add ImGui library
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Core")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Model_Loaders")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Render_Systems")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Layers")
target_include_directories(VectorVertex PUBLIC "src/Render/vendor/imgui")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
