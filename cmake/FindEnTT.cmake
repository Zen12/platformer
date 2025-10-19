include(FetchContent)

# Only fetch once
if (NOT TARGET EnTT::EnTT)
    FetchContent_Declare(
        EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.14.0  # Or latest release tag
    )

    FetchContent_MakeAvailable(EnTT)

    # EnTT is header-only, so it likely already creates EnTT::EnTT target
    # If not, we can access includes via ${entt_SOURCE_DIR}/src
    if (NOT TARGET EnTT::EnTT AND TARGET EnTT)
        add_library(EnTT::EnTT ALIAS EnTT)
    endif()

    # Optional: expose include dirs as a variable
    set(ENTT_INCLUDE_DIRS ${entt_SOURCE_DIR}/src CACHE INTERNAL "EnTT include dirs")
endif ()