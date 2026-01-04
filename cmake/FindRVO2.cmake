include(FetchContent)

# Only fetch once
if (NOT TARGET RVO)
    FetchContent_Declare(
        rvo2
        GIT_REPOSITORY https://github.com/snape/RVO2.git
        GIT_TAG main
    )

    FetchContent_MakeAvailable(rvo2)

    # Expose include directories
    set(RVO2_INCLUDE_DIRS
        ${rvo2_SOURCE_DIR}/src
        CACHE INTERNAL "RVO2 include dirs"
    )
endif()
