include(FetchContent)

# Only fetch once
if (NOT TARGET assimp)
    # Suppress the deprecation warning for FetchContent_Populate
    cmake_policy(SET CMP0169 OLD)

    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3  # Latest stable version
    )

    # Assimp options - disable unnecessary features to speed up build
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)

    FetchContent_GetProperties(assimp)
    if(NOT assimp_POPULATED)
        FetchContent_Populate(assimp)
        add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
    endif()

    # Assimp creates its own target, so we don't need to create an alias
    # The target is named 'assimp'

    # Optional: expose include dirs as a variable
    set(ASSIMP_INCLUDE_DIRS ${assimp_SOURCE_DIR}/include ${assimp_BINARY_DIR}/include CACHE INTERNAL "Assimp include dirs")
endif ()
