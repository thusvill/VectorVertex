cmake_minimum_required(VERSION 3.20)
project(VectorVertexEditor LANGUAGES C CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "src/*.h")

add_executable(VectorVertexEditor ${SOURCES})

target_link_libraries(VectorVertexEditor VectorVertexEngine)

target_include_directories(VectorVertexEditor PRIVATE
    ../Engine/src
    
)