include(FetchContent)

# Only fetch once
if (NOT TARGET Minitrace::Minitrace)
    FetchContent_Declare(
        minitrace
        GIT_REPOSITORY https://github.com/hrydgard/minitrace.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(minitrace)

    # Minitrace is not a CMake project, so we create a library target manually
    add_library(minitrace_lib STATIC
        ${minitrace_SOURCE_DIR}/minitrace.c
        ${minitrace_SOURCE_DIR}/minitrace.h
    )

    target_include_directories(minitrace_lib PUBLIC ${minitrace_SOURCE_DIR})

    # Enable profiling in debug builds - must be defined for both library and consumers
    target_compile_definitions(minitrace_lib PUBLIC $<$<CONFIG:Debug>:MTR_ENABLED>)

    # Create namespaced alias
    add_library(Minitrace::Minitrace ALIAS minitrace_lib)

    # Expose include dirs as a variable
    set(MINITRACE_INCLUDE_DIRS ${minitrace_SOURCE_DIR} CACHE INTERNAL "Minitrace include dirs")
endif()
