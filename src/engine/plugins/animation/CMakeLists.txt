# Engine Animation Module
# Animation systems and components

add_library(engine-animation INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-animation INTERFACE ${SOURCES})

target_include_directories(engine-animation INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(engine-animation INTERFACE
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-guid
    engine-core
)
