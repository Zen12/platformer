# Engine Font Module
# Font loading and rendering

add_library(engine-font STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-font PRIVATE ${SOURCES})

target_include_directories(engine-font PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-font PUBLIC
    GLM::GLM
    freetype
    engine-core
)

if(PLATFORM_DESKTOP)
    target_link_libraries(engine-font PUBLIC GLEW::GLEW)
endif()

target_compile_features(engine-font PUBLIC cxx_std_17)
