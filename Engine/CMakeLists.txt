cmake_minimum_required(VERSION 3.20)
project(VectorVertexEngine LANGUAGES C CXX)



find_package(PkgConfig REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(X11 REQUIRED)


add_library(VectorVertexEngine STATIC)

set(FBXSDK_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/fbxsdk)

find_library(FBXSDK_LIB
    NAMES fbxsdk
    PATHS ${FBXSDK_ROOT}/lib/release
    NO_DEFAULT_PATH
    REQUIRED
)

target_include_directories(VectorVertexEngine PUBLIC
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
add_subdirectory(src/vendor/spdlog)

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


set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE) 
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE) 
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE) 
add_subdirectory(src/vendor/yaml-cpp)

target_include_directories(VectorVertexEngine PRIVATE ${yaml-cpp_SOURCE_DIR}/include)


target_link_options(VectorVertexEngine INTERFACE 
    "-Wl,--copy-dt-needed-entries"
    "-Wl,-rpath-link,/usr/lib"
    "-Wl,--allow-shlib-undefined"
    )


target_link_libraries(VectorVertexEngine
    PUBLIC
        ${FBXSDK_LIB}    
        imgui            
        EnTT::EnTT
        tinyobjloader
        yaml-cpp::yaml-cpp
        spdlog::spdlog
        glfw             
        vulkan      
        shaderc_shared    
        SPIRV-Tools
        #LibXml2::LibXml2
        xml2
        X11               
    PRIVATE
        pthread
        dl
)




