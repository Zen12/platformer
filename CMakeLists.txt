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
file(GLOB_RECURSE HEADER_FILES
    "src/*.hpp"
)

# Find all source files
file(GLOB_RECURSE SOURCE_FILES
	"src/*.cpp"
)

# Enable warnings as errors
if(MSVC)
    add_compile_options(/W4 /WX)  # /WX treats warnings as errors in MSVC
else()
    add_compile_options(-Wall -Wextra -Werror)  # -Werror for GCC/Clang
endif()

# This command adds a build target (executable or library, use add_library for libraries).
# The first word is the name of the executable and everything after that is the source files you want to include in
# this new target.
add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# This links libraries against a target, the target being the executable we just added.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw GLM::GLM freetype yaml-cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
