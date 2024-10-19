cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)
project(VectorVertex)

# Find required packages
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(yaml-cpp REQUIRED)

# Set the path to the FBX SDK (inside the src folder)
set(FBX_SDK_DIR "${CMAKE_SOURCE_DIR}/src/vendor/fbx")

# Include directories
include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}

    # FBX SDK directory (in src)
    ${FBX_SDK_DIR}/include  # Path to fbxsdk.h inside the src folder

    # Core directories
    "src/Core"
    "src/Core/Math"
    "src/Core/Utils"

    # Plattform directories
    "src/Plattform"
    "src/Plattform/Vulkan"
    "src/Plattform/Vulkan/Render_Systems"

    # Render directories
    "src/Render"

    # VectorVertex directories
    "src/VectorVertex"
    "src/VectorVertex/Events"
    "src/VectorVertex/Layers"
    "src/VectorVertex/Panels"
    "src/VectorVertex/Scene"

    # Vendor libraries
    "src/vendor"
    "src/vendor/entt"
    "src/vendor/imgui"
    "src/vendor/imgui/backends/vulkan"
    "src/vendor/ImGuizmo"
    "src/vendor/yaml-cpp/include"
)

# Collect source files
file(GLOB_RECURSE SOURCES 
    "src/Core/**.cpp"
    "src/Core/**.h"
    "src/Plattform/**.cpp"
    "src/Plattform/**.hpp"
    "src/Render/**.cpp"
    "src/Render/**.hpp"
    "src/VectorVertex/**.cpp"
    "src/VectorVertex/**.hpp"
    "src/vendor/imgui/*.cpp"
    "src/vendor/ImGuizmo/*.cpp"
)

# Copy the entire Resources directory to the build output
file(COPY ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_BINARY_DIR})

# Add executable
add_executable(VectorVertex main.cpp ${SOURCES})

# Link libraries
target_link_libraries(VectorVertex 
    glfw 
    fmt 
    ${Vulkan_LIBRARIES}
    yaml-cpp
)

# Find the FBX SDK library (in src/vendor/fbx/lib/debug or release)
find_library(FBX_LIBRARY fbxsdk HINTS ${FBX_SDK_DIR}/lib/debug ${FBX_SDK_DIR}/lib/release)

# Link the FBX SDK library
if (FBX_LIBRARY)
    target_link_libraries(VectorVertex ${FBX_LIBRARY})
else()
    message(FATAL_ERROR "FBX SDK library not found!")
endif()

# Add debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -w -g")
