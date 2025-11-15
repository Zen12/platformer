# Only fetch if not already available
if(NOT TARGET freetype)
    FetchContent_Declare(
        freetype
        URL https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.gz
    )

    FetchContent_MakeAvailable(freetype)

    # Create alias for RmlUi compatibility
    if(TARGET freetype AND NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype)
    endif()
endif()


# stb
if(NOT TARGET stb)
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(stb)
endif()