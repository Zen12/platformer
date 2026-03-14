# Engine Utils Module
# Math utilities and helper functions

add_library(engine-utils INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-utils INTERFACE ${SOURCES})

target_include_directories(engine-utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-utils INTERFACE
    GLM::GLM
)
