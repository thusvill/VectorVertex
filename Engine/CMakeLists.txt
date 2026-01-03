cmake_minimum_required(VERSION 3.20)
project(VectorVertexEngine LANGUAGES C CXX)


add_library(VectorVertexEngine STATIC)

set(FBXSDK_ROOT ${CMAKE_SOURCE_DIR}/src/vendor/fbxsdk)

find_library(FBXSDK_LIB
    NAMES fbxsdk
    PATHS ${FBXSDK_ROOT}/lib/release
    NO_DEFAULT_PATH
)

target_include_directories(VectorVertexEngine PRIVATE
    ${FBXSDK_ROOT}/include
)

add_subdirectory(src/vendor/entt)



file(GLOB_RECURSE VECTOR_VERTEX_SOURCES
    "src/*.cpp"
    "src/*.c"
)

file(GLOB_RECURSE VECTOR_VERTEX_HEADERS
    "src/*.h"
    "src/*.hpp"
)

link_directories(
    ${FBXSDK_ROOT}/lib/release
)


target_sources(VectorVertexEngine
    PRIVATE ${VECTOR_VERTEX_SOURCES}
    PUBLIC  ${VECTOR_VERTEX_HEADERS}
)

target_include_directories(VectorVertexEngine PRIVATE src/vendor/glm)
target_include_directories(VectorVertexEngine PRIVATE src/vendor/spdlog/include)

target_include_directories(VectorVertexEngine
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_library(imgui
    src/vendor/imgui/imgui.cpp
    src/vendor/imgui/imgui_draw.cpp
    src/vendor/imgui/imgui_tables.cpp
    src/vendor/imgui/imgui_widgets.cpp
    src/vendor/imgui/backends/imgui_impl_glfw.cpp
    src/vendor/imgui/backends/imgui_impl_vulkan.cpp
)
add_library(tinyobjloader INTERFACE)

target_include_directories(imgui PUBLIC
    src/vendor/imgui
    src/vendor/imgui/backends
)

target_link_libraries(imgui PUBLIC glfw vulkan)
target_include_directories(tinyobjloader INTERFACE src/vendor/tinyobjloader)

add_subdirectory(src/vendor/yaml-cpp)

target_include_directories(VectorVertexEngine PRIVATE ${yaml-cpp_SOURCE_DIR}/include)





target_link_libraries(VectorVertexEngine
    ${FBXSDK_LIB}
    pthread
    dl
    EnTT::EnTT
    imgui
    tinyobjloader
    yaml-cpp
)




