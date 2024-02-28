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

# Collect all .cpp, .h, and .hpp files in the source directory
file(GLOB_RECURSE SOURCES "src/Render/*.cpp" "src/Render/*.hpp" "src/Render/*.h" "src/Core/*.hpp" "src/Core/*.cpp")

# Add executable and link libraries
add_executable(VectorVertex main.cpp ${SOURCES})
target_link_libraries(VectorVertex glfw ${Vulkan_LIBRARIES})
