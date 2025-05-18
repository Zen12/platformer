include(FetchContent)

FetchContent_Declare(
  glew
  GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
  GIT_TAG master
)

FetchContent_MakeAvailable(glew)

# Choose between static or shared library
set(GLEW_TARGET libglew_static) # or libglew_shared libglew_static

# Create an alias for consistency
add_library(GLEW::GLEW ALIAS ${GLEW_TARGET})

# Set include directories
set(GLEW_INCLUDE_DIRS ${glew_SOURCE_DIR}/include CACHE INTERNAL "GLEW include dirs")
