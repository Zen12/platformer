# Engine Project Module
# Project asset loading

add_library(engine-project INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-project INTERFACE ${SOURCES})

target_include_directories(engine-project INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-project INTERFACE
    yaml-cpp
    engine-core
)
