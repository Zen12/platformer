# Game Plugin
# Game-specific components, systems, and actions (health, player controller, spawner)

add_library(engine-game INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-game INTERFACE ${SOURCES})
target_sources(engine-game INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/game_plugin.hpp)

target_include_directories(engine-game INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-game INTERFACE
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-core
    engine-scene
    engine-navigation
    engine-renderer
    engine-ai
    engine-animation
    engine-prefab
    engine-system
)

target_compile_features(engine-game INTERFACE cxx_std_17)
