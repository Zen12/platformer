# Engine IK Module
# Inverse kinematics systems and components

add_library(engine-ik STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-ik PRIVATE ${SOURCES})
target_sources(engine-ik PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ik_plugin.hpp)

target_include_directories(engine-ik PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-ik PRIVATE
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-core
    engine-system
    engine-font
    rmlui_core
)

# Cross-module dependencies
target_include_directories(engine-ik PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../navigation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../animation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../ai/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
