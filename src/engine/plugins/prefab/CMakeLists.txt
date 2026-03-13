# Engine Prefab Module
# Prefab asset definitions

add_library(engine-prefab INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-prefab INTERFACE ${SOURCES})

target_include_directories(engine-prefab INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(engine-prefab INTERFACE
    yaml-cpp
    engine-core
)
