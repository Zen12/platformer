cmake_minimum_required(VERSION 3.11...3.30)

project(Engine-Demo)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set minimum policy version for compatibility
cmake_policy(SET CMP0000 NEW)

# Dependencies
find_package(YAMLCPP REQUIRED)
find_package(GLM REQUIRED)
find_package(EnTT REQUIRED)
find_package(RmlUi REQUIRED)
find_package(FREETYPE REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 3.3 REQUIRED)

# --- Source files ---
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# --- Create target ---
add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# --- Compiler flags ---
if(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/W4 /WX /Zi>
            $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )
else()
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer -g>
            $<$<CONFIG:Release>:-Wall -Wextra -Wpedantic -Wnull-dereference -Wuninitialized -Werror -O3 -DNDEBUG>
    )
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

# --- Link libraries ---
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw GLM::GLM freetype yaml-cpp EnTT::EnTT rmlui_core assimp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

# --- Output configuration ---
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
)

file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/output/assets")
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}/output")

# --- Run target ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(run
            COMMAND "${CMAKE_BINARY_DIR}/output/${CMAKE_PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            DEPENDS ${CMAKE_PROJECT_NAME}
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/output"
    )
else()
    add_custom_target(run
            COMMAND "${CMAKE_BINARY_DIR}/output/${CMAKE_PROJECT_NAME}"
            DEPENDS ${CMAKE_PROJECT_NAME}
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/output"
    )
endif()
