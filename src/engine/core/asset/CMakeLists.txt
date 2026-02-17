# Engine Asset Module
# Asset management, metadata parsing, and loaders

add_library(engine-asset STATIC)

# Collect source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-asset PRIVATE ${SOURCES})

target_include_directories(engine-asset PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependencies
target_link_libraries(engine-asset PUBLIC
    yaml-cpp
    engine-guid
)

# Platform-specific dependencies for texture loader
if(PLATFORM_DESKTOP)
    target_link_libraries(engine-asset PUBLIC GLEW::GLEW)
    target_include_directories(engine-asset PRIVATE ${stb_SOURCE_DIR})
endif()

if(PLATFORM_WEB)
    target_include_directories(engine-asset PRIVATE ${stb_SOURCE_DIR})
endif()

target_compile_features(engine-asset PUBLIC cxx_std_17)

# Tests
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
