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
find_package(Box2D REQUIRED)
find_package(Spine REQUIRED)

# Find all header files
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")

# Find all source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Create executable target
add_executable(${CMAKE_PROJECT_NAME} ${USE_MACOSX_BUNDLE}
        ${HEADER_FILES} ${SOURCE_FILES} ${SPINE_CPP_SOURCES})

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

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw GLM::GLM freetype yaml-cpp box2d spine-cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR} ${SPINE_CPP_SOURCES})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
)

file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/output/assets")

file(COPY "${CMAKE_SOURCE_DIR}/assets"
        DESTINATION "${CMAKE_BINARY_DIR}/output")

add_custom_target(run
        COMMAND "${CMAKE_BINARY_DIR}/output/${CMAKE_PROJECT_NAME}"
        DEPENDS ${CMAKE_PROJECT_NAME}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/output"
)
