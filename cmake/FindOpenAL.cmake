include(FetchContent)

# For web builds, Emscripten provides OpenAL emulation
if(PLATFORM_WEB)
    message(STATUS "OpenAL: Using Emscripten's built-in OpenAL emulation")
    # Create an interface library for consistent linking
    add_library(OpenAL::OpenAL INTERFACE IMPORTED)
    return()
endif()

# Desktop builds use OpenAL-Soft
FetchContent_Declare(
    openal-soft
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG        1.24.2
)

# Disable building utilities and examples
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(openal-soft)

# Create alias for consistent target naming
if(NOT TARGET OpenAL::OpenAL)
    add_library(OpenAL::OpenAL ALIAS OpenAL)
endif()
