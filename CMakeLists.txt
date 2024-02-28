cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)
project(VectorVertex)

# Add GLFW library
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

# Add Vulkan library
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories("src/Render/Model_Loaders")
include_directories("src/Render/Render_Systems")
include_directories("src/Render/vendor")    
include_directories("src/Render/Layers")

# Collect all .cpp, .h, and .hpp files in the source directory
file(GLOB_RECURSE SOURCES "src/Render/*.cpp" "src/Core/*.cpp")

# Add executable and link libraries
add_executable(VectorVertex main.cpp ${SOURCES})

target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Core")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Model_Loaders")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Render_Systems")
target_include_directories(VectorVertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Layers")
target_link_libraries(VectorVertex glfw fmt ${Vulkan_LIBRARIES})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")