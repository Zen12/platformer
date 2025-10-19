
# Suppress the deprecation warning for FetchContent_Populate
cmake_policy(SET CMP0169 OLD)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG        a83cd31548b19d50f3f983b069dceb4f4d50756d
)

FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
  FetchContent_Populate(yaml-cpp)


  add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()