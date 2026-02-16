# Engine Profiler Module
# Wrapper around minitrace for Chrome Tracing profiling

add_library(engine-profiler INTERFACE)

target_include_directories(engine-profiler INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Minitrace dependency (from parent)
target_include_directories(engine-profiler INTERFACE
    ${MINITRACE_INCLUDE_DIRS}
)

# Link minitrace only in debug builds (MTR_ENABLED is defined there)
target_link_libraries(engine-profiler INTERFACE
    $<$<CONFIG:Debug>:Minitrace::Minitrace>
)
