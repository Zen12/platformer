# Engine Scene Module
# Scene management and loading

add_library(engine-scene STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-scene PRIVATE ${SOURCES})

target_include_directories(engine-scene PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(engine-scene PUBLIC
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    engine-guid
    engine-core
    engine-system
    engine-navigation
    engine-renderer
)

target_compile_features(engine-scene PUBLIC cxx_std_17)
