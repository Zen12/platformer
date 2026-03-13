# Engine Renderer Module
# Rendering, materials, meshes, textures, UI

add_library(engine-renderer STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-renderer PRIVATE ${SOURCES})
target_sources(engine-renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/renderer_plugin.hpp)

target_include_directories(engine-renderer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-renderer PUBLIC
    GLM::GLM
    yaml-cpp
    EnTT::EnTT
    freetype
    rmlui_core
    assimp
    RVO
    minitrace_lib
    engine-core
    engine-system
    engine-font
    engine-math
    engine-profiler
)

# Cross-module dependencies
target_include_directories(engine-renderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../animation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../navigation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../audio/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../ai/src
    ${CMAKE_SOURCE_DIR}/src/game/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../prefab/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../profiler/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

if(PLATFORM_DESKTOP)
    target_link_libraries(engine-renderer PRIVATE GLEW::GLEW OpenGL::GL OpenAL::OpenAL)
    target_include_directories(engine-renderer PRIVATE ${stb_SOURCE_DIR} ${openal-soft_SOURCE_DIR}/include)
endif()

if(VIDEO_RECORDING)
    target_link_libraries(engine-renderer PRIVATE FFMPEG::avcodec FFMPEG::avformat FFMPEG::avutil FFMPEG::swscale)
    target_include_directories(engine-renderer PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_compile_definitions(engine-renderer PRIVATE VIDEO_RECORDING)
endif()
