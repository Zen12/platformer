# Engine Renderer Module
# Rendering, materials, meshes, textures, UI

add_library(engine-renderer STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-renderer PRIVATE ${SOURCES})

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
    engine-guid
    engine-asset
    engine-system
    engine-font
    engine-math
    engine-profiler
)

# Cross-module dependencies - added separately to handle circular dependencies
target_include_directories(engine-renderer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../navigation/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../audio/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../fsm/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../prefab/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../profiler/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../ai_pathfinder/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

if(PLATFORM_DESKTOP)
    target_link_libraries(engine-renderer PUBLIC GLEW::GLEW OpenGL::GL OpenAL::OpenAL)
    target_include_directories(engine-renderer PRIVATE ${stb_SOURCE_DIR} ${openal-soft_SOURCE_DIR}/include)
endif()

if(VIDEO_RECORDING)
    target_link_libraries(engine-renderer PUBLIC FFMPEG::avcodec FFMPEG::avformat FFMPEG::avutil FFMPEG::swscale)
    target_include_directories(engine-renderer PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_compile_definitions(engine-renderer PRIVATE VIDEO_RECORDING)
endif()

target_compile_features(engine-renderer PUBLIC cxx_std_17)
