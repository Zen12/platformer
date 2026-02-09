include(FetchContent)

# Only fetch once
if (NOT TARGET xsimd)
    FetchContent_Declare(
        xsimd
        GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
        GIT_TAG 13.0.0
    )

    FetchContent_MakeAvailable(xsimd)

    # xsimd creates xsimd target, create namespaced alias
    if (NOT TARGET xsimd::xsimd AND TARGET xsimd)
        add_library(xsimd::xsimd ALIAS xsimd)
    endif()

    # Expose include dirs as a variable
    set(XSIMD_INCLUDE_DIRS ${xsimd_SOURCE_DIR}/include CACHE INTERNAL "xsimd include dirs")
endif ()