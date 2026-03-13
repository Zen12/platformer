# Engine AI Module
# Behavior tree system and AI components

add_library(engine-ai INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-ai INTERFACE ${SOURCES})
target_sources(engine-ai INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ai_plugin.hpp)

target_include_directories(engine-ai INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-ai INTERFACE
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-core
    engine-navigation
)

target_compile_features(engine-ai INTERFACE cxx_std_17)
