# Engine Mesh Generator Module
# Procedural mesh generation from .meshgen YAML assets using Manifold CSG

add_library(engine-mesh-generator INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-mesh-generator INTERFACE ${SOURCES})
target_sources(engine-mesh-generator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mesh_generator_plugin.hpp)

target_include_directories(engine-mesh-generator INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-mesh-generator INTERFACE
    yaml-cpp
    GLM::GLM
    engine-core
    engine-math
    engine-bezier
    engine-renderer
    manifold
)
