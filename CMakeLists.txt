cmake_minimum_required(VERSION 3.28)

project(Platformer)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 3.3 REQUIRED)
find_package(glm REQUIRED)


# stb
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(stb)

include(FetchContent)

FetchContent_Declare(
    freetype
    URL https://download.savannah.gnu.org/releases/freetype/freetype-2.13.0.tar.gz
)

FetchContent_MakeAvailable(freetype)


# Find all header files
file(GLOB_RECURSE HEADER_FILES
    "src/*.h"
)

# Find all source files
file(GLOB_RECURSE SOURCE_FILES
	"src/*.cpp"
)

# This command adds a build target (executable or library, use add_library for libraries).
# The first word is the name of the executable and everything after that is the source files you want to include in
# this new target.
add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# This links libraries against a target, the target being the executable we just added.
target_link_libraries(${CMAKE_PROJECT_NAME} OpenGL::GL GLEW::glew glfw glm::glm freetype)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
