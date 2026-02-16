# Engine Navigation Module
# Navmesh, pathfinding, and crowd simulation

add_library(engine-navigation STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-navigation PRIVATE ${SOURCES})

target_include_directories(engine-navigation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-navigation PUBLIC
    GLM::GLM
    RVO
)

# Cross-module dependencies
target_include_directories(engine-navigation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/src
)

target_compile_features(engine-navigation PUBLIC cxx_std_17)
