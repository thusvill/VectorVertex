cmake_minimum_required(VERSION 3.20)
project(VectorVertexEditor LANGUAGES C CXX)

set(EDITOR_VENDOR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor")



file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "src/*.h")
list(APPEND SOURCES "${EDITOR_VENDOR_PATH}/ImGuizmo/ImGuizmo.cpp")

add_executable(VectorVertexEditor ${SOURCES})



target_include_directories(VectorVertexEditor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ../Engine/src
    "${EDITOR_VENDOR_PATH}"

    "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src/vendor/entt/single_include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src/vendor/yaml-cpp/include"
    

)



target_link_libraries(VectorVertexEditor 
VectorVertexEngine
 )