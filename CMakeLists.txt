cmake_minimum_required(VERSION 3.28)

project(Platformer)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(GLFW3 3.3 REQUIRED)
find_package(FREETYPE REQUIRED)
find_package(YAMLCPP REQUIRED)

# Find all header files
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")

# Find all source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Create executable target
add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Compiler warnings and errors per compiler and configuration

if(MSVC)
    # MSVC flags for warnings and optimization/debug
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/W4 /WX /Zi>
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )
else()
    # GCC/Clang flags for warnings/errors and optimization/debug
    # removed -Wshadow because of YAML lib
    # removed -fsanitize=leak because Apple doesn't support 
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>: -fsanitize=address,undefined -fno-omit-frame-pointer -g>
        $<$<CONFIG:Release>:-Wall -Wextra -Wpedantic -Wnull-dereference -Wuninitialized -Werror -O3 -DNDEBUG>
    )

    # Link sanitizer runtime only for Debug
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined> 
    )
endif()

# Link libraries to your target
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw GLM::GLM freetype yaml-cpp)

# Include directories (example: external stb lib, adjust as needed)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

# Define macro with asset path
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
