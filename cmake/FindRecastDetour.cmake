include(FetchContent)

# Only fetch once
if (NOT TARGET Recast)
    # Set CMake policy for compatibility
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

    FetchContent_Declare(
        recastnavigation
        GIT_REPOSITORY https://github.com/recastnavigation/recastnavigation.git
        GIT_TAG v1.6.0  # Latest stable release
    )

    # Configure Recast & Detour options
    set(RECASTNAVIGATION_DEMO OFF CACHE BOOL "" FORCE)
    set(RECASTNAVIGATION_TESTS OFF CACHE BOOL "" FORCE)
    set(RECASTNAVIGATION_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(RECASTNAVIGATION_STATIC ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(recastnavigation)

    # Expose include directories
    set(RECAST_INCLUDE_DIRS
        ${recastnavigation_SOURCE_DIR}/Recast/Include
        ${recastnavigation_SOURCE_DIR}/Detour/Include
        ${recastnavigation_SOURCE_DIR}/DetourCrowd/Include
        ${recastnavigation_SOURCE_DIR}/DetourTileCache/Include
        CACHE INTERNAL "Recast & Detour include dirs"
    )
endif()
