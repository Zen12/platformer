# Engine Navigation Module
# Navmesh, pathfinding, and crowd simulation

add_library(engine-navigation STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-navigation PRIVATE ${SOURCES})
target_sources(engine-navigation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/navigation_plugin.hpp)

target_include_directories(engine-navigation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-navigation PUBLIC
    GLM::GLM
    RVO
    EnTT::EnTT
    engine-core
)

# Cross-module dependencies
target_include_directories(engine-navigation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene/src
)
