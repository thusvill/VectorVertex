cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)
project(VectorVertex)

# Find required packages
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(yaml-cpp REQUIRED)

# Set the path to the FBX SDK
set(FBX_SDK_DIR "src/Render/Model_Loaders/fbx")

# Include directories
include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    "src/Render/Model_Loaders"
    "src/Render/Render_Systems"
    "src/Render/vendor"
    "src/Core/vendor"
    "src/Core/Panels"
    "src/Core/Scene"
    "src/Render/Layers"
    "src/Render/vendor/imgui"
    "src/vendor/entt"
    ${FBX_SDK_DIR}/include
)

# Collect all .cpp, .h, and .hpp files in the source directory
file(GLOB IMGUI_SOURCES "src/Render/vendor/imgui/*.cpp")

# Collect source files
# Copy the entire Resources directory to the build output
file(COPY ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCES 
    ${IMGUI_SOURCES}
    "src/Render/**.cpp" 
    "src/Core/Scene/**.cpp" 
    "src/Core/Scene/**.hpp" 
    "src/Core/Panels/**.cpp" 
    "src/Core/Panels/**.hpp"  
    "src/Core/**.cpp" 
    "src/Render/Layers/*.cpp" 
    "src/Render/Layers/Editor_Layer.cpp" 
    "src/Render/Layers/Editor_Layer.hpp" 
    "src/Render/Layers/Imgui_Layer.cpp" 
    "src/Render/Layers/Imgui_Layer.hpp"
)

# Add executable
add_executable(VectorVertex main.cpp ${SOURCES})

# Link libraries
target_link_libraries(VectorVertex 
    glfw 
    fmt 
    ${Vulkan_LIBRARIES}
)

# Link the FBX SDK library
find_library(FBX_LIBRARY fbxsdk HINTS ${FBX_SDK_DIR}/lib/debug)
target_link_libraries(VectorVertex ${FBX_LIBRARY} yaml-cpp)

# Add ImGui library
target_include_directories(VectorVertex PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Render"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Model_Loaders"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Render_Systems"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Layers"
    "src/Render/vendor/imgui"
)

# Set debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -w -g")
