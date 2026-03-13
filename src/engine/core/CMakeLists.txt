# Engine Core Module
# Merged: asset management, FSM system, AI & pathfinding

add_library(engine-core STATIC)

# Collect source files (cpp + hpp from all three submodules)
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-core PRIVATE ${SOURCES})
target_sources(engine-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core_plugin.hpp)

# Include directories - one per submodule for backward compatibility
# This ensures existing #include directives continue to work unchanged
target_include_directories(engine-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asset
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fsm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/guid
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resource
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Dependencies (union of all three, minus engine-renderer/engine-scene/engine-system to avoid circular deps)
target_link_libraries(engine-core PUBLIC
    yaml-cpp
    GLM::GLM
    EnTT::EnTT
)

# Platform-specific dependencies (from engine-asset)
if(PLATFORM_DESKTOP)
    target_link_libraries(engine-core PUBLIC GLEW::GLEW)
    target_include_directories(engine-core PRIVATE ${stb_SOURCE_DIR})
endif()

if(PLATFORM_WEB)
    target_include_directories(engine-core PRIVATE ${stb_SOURCE_DIR})
endif()

target_compile_features(engine-core PUBLIC cxx_std_17)

# Tests
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
