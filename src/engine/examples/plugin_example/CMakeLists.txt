# Example Plugin
# Demonstrates the plugin auto-registration pattern

add_library(engine-example-plugin INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-example-plugin INTERFACE ${SOURCES})

target_include_directories(engine-example-plugin INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-example-plugin INTERFACE
    yaml-cpp
    engine-core
)
