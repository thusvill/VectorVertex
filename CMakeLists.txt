cmake_minimum_required(VERSION 3.20)
project(VectorVertex-Workspace)

# Add Engine first so the target exists
add_subdirectory(Engine)

# Add Editor second so it can "see" the Engine target
add_subdirectory(Editor)