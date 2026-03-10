# Engine GUID Module
# UUID generation and parsing utilities

add_library(engine-guid STATIC)

# Collect source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-guid PRIVATE ${SOURCES})

target_include_directories(engine-guid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependencies
target_link_libraries(engine-guid PUBLIC yaml-cpp)

# Tests
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
