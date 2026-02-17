# Engine FSM Module
# Finite State Machine system

add_library(engine-fsm INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-fsm INTERFACE ${SOURCES})

target_include_directories(engine-fsm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-fsm INTERFACE
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-guid
    engine-asset
    engine-system
    engine-renderer
    engine-scene
)
