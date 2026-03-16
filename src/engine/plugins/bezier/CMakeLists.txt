# Engine Bezier Module
# Bezier curve asset and evaluation utilities

add_library(engine-bezier INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-bezier INTERFACE ${SOURCES})
target_sources(engine-bezier INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/bezier_plugin.hpp)

target_include_directories(engine-bezier INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-bezier INTERFACE
    yaml-cpp
    GLM::GLM
    engine-core
    engine-math
)
