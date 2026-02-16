include(FetchContent)

# Only fetch once
if (NOT TARGET doctest::doctest)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
    )

    FetchContent_MakeAvailable(doctest)

    # Expose include dirs as a variable
    set(DOCTEST_INCLUDE_DIRS ${doctest_SOURCE_DIR}/doctest CACHE INTERNAL "Doctest include dirs")
endif()
