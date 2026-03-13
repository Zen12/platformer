# Engine Scene Module
# Scene management and loading

add_library(engine-scene STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-scene PRIVATE ${SOURCES})
target_sources(engine-scene PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/scene_plugin.hpp)

target_include_directories(engine-scene PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-scene PUBLIC
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-core
    engine-system
    engine-navigation
    engine-renderer
    engine-audio
)

# Cross-module dependencies
target_include_directories(engine-scene PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../animation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../prefab/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../audio/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../profiler/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
