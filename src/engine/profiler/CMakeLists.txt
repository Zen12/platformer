# Engine Profiler Module
# Wrapper around minitrace for Chrome Tracing profiling

add_library(engine-profiler INTERFACE)

# Collect source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-profiler INTERFACE ${SOURCES})

target_include_directories(engine-profiler INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MINITRACE_INCLUDE_DIRS}
)

# Dependencies - minitrace only in debug builds
target_link_libraries(engine-profiler INTERFACE
    $<$<CONFIG:Debug>:Minitrace::Minitrace>
)

# Tests
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
