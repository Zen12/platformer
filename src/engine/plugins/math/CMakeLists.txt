# Engine Math Module
# GLM YAML serialization utilities

add_library(engine-math INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-math INTERFACE ${SOURCES})

target_include_directories(engine-math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-math INTERFACE
    GLM::GLM
    yaml-cpp
)
