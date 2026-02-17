# Engine AI & Pathfinding Module
# Behavior trees, A* pathfinding, and AI utilities

add_library(engine-ai INTERFACE)

# Collect source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-ai INTERFACE ${SOURCES})

target_include_directories(engine-ai INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

# Dependencies
target_link_libraries(engine-ai INTERFACE
    GLM::GLM
    EnTT::EnTT
    yaml-cpp
    engine-guid
)

# Tests
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
