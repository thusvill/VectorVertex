# Create the static library
add_library(VectorVertexEngine STATIC)


file(GLOB_RECURSE VECTOR_VERTEX_SOURCES
    "src/*.cpp"
    "src/*.c"
)

file(GLOB_RECURSE VECTOR_VERTEX_HEADERS
    "src/*.h"
    "src/*.hpp"
)


target_sources(VectorVertexEngine
    PRIVATE ${VECTOR_VERTEX_SOURCES}
    PUBLIC  ${VECTOR_VERTEX_HEADERS}
)


target_include_directories(VectorVertexEngine
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
