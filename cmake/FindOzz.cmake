include(FetchContent)

# Only fetch once
if (NOT TARGET ozz_base)
    FetchContent_Declare(
        ozz
        GIT_REPOSITORY https://github.com/guillaumeblanc/ozz-animation.git
        GIT_TAG        0.15.0
    )

    # Configure ozz-animation options - disable unnecessary features
    set(ozz_build_samples OFF CACHE BOOL "" FORCE)
    set(ozz_build_howtos OFF CACHE BOOL "" FORCE)
    set(ozz_build_tests OFF CACHE BOOL "" FORCE)
    set(ozz_build_simd_ref OFF CACHE BOOL "" FORCE)
    set(ozz_build_msvc_rt_dll ON CACHE BOOL "" FORCE)
    set(ozz_build_gltf OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(ozz)

    # Ozz creates its own targets: ozz_base, ozz_animation, ozz_animation_offline
    # No need to create aliases

    # Optional: expose include dirs as a variable
    set(OZZ_INCLUDE_DIRS ${ozz_SOURCE_DIR}/include CACHE INTERNAL "Ozz include dirs")
endif ()
