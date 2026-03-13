# Engine Audio Module
# Audio playback and management

add_library(engine-audio INTERFACE)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-audio INTERFACE ${SOURCES})
target_sources(engine-audio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/audio_plugin.hpp)

target_include_directories(engine-audio INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-audio INTERFACE
    GLM::GLM
    yaml-cpp
    engine-guid
    engine-core
)

if(PLATFORM_DESKTOP)
    target_link_libraries(engine-audio INTERFACE OpenAL::OpenAL)
endif()
