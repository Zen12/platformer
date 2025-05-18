include(FetchContent)

# Only fetch once
if (NOT TARGET glm)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8  # Or latest release tag
    )

    FetchContent_MakeAvailable(glm)

    # Create an alias target for standard usage
    add_library(GLM::GLM ALIAS glm)

    # Optional: expose include dirs as a variable
    set(GLM_INCLUDE_DIRS ${glm_SOURCE_DIR} CACHE INTERNAL "GLM include dirs")
endif ()