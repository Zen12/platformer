# Engine Animation Module
# Animation systems and components

add_library(engine-animation STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-animation PRIVATE ${SOURCES})
target_sources(engine-animation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/animation_plugin.hpp)

target_include_directories(engine-animation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-animation PRIVATE
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    assimp
    engine-guid
    engine-core
    engine-system
    engine-font
    rmlui_core
)

# Cross-module dependencies
target_include_directories(engine-animation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../navigation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../ik/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../ai/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
