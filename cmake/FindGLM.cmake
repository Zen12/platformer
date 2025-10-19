include(FetchContent)

# Only fetch once
if (NOT TARGET glm)
    # Suppress the deprecation warning for FetchContent_Populate
    cmake_policy(SET CMP0169 OLD)

    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1  # Newer version with better CMake support
    )

    FetchContent_GetProperties(glm)
    if(NOT glm_POPULATED)
        FetchContent_Populate(glm)

        # Patch the CMakeLists.txt to use a compatible cmake_minimum_required
        file(READ "${glm_SOURCE_DIR}/CMakeLists.txt" GLM_CMAKE)
        string(REGEX REPLACE "cmake_minimum_required\\(VERSION [0-9.]+\\)" "cmake_minimum_required(VERSION 3.5)" GLM_CMAKE "${GLM_CMAKE}")
        file(WRITE "${glm_SOURCE_DIR}/CMakeLists.txt" "${GLM_CMAKE}")

        add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
    endif()

    # Create an alias target for standard usage
    add_library(GLM::GLM ALIAS glm)

    # Optional: expose include dirs as a variable
    set(GLM_INCLUDE_DIRS ${glm_SOURCE_DIR} CACHE INTERNAL "GLM include dirs")
endif ()