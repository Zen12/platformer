include(FetchContent)

# Only fetch once
if (NOT TARGET rmlui_core)
    FetchContent_Declare(
        RmlUi
        GIT_REPOSITORY https://github.com/mikke89/RmlUi.git
        GIT_TAG        6.0  # Latest stable version
    )

    # Configure RmlUi options
    set(RMLUI_BACKEND "GLFW_GL3" CACHE STRING "" FORCE)
    set(BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(ENABLE_PRECOMPILED_HEADERS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(RmlUi)

    # RmlUi 6.0 creates targets: rmlui_core, rmlui_debugger
    # The backend creates: rmlui_backend_glfw_gl3
endif()