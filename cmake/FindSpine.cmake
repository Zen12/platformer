include(FetchContent)

FetchContent_Declare(
        spine_cpp
        GIT_REPOSITORY https://github.com/EsotericSoftware/spine-runtimes.git
        GIT_TAG 4.2
)

FetchContent_MakeAvailable(spine_cpp)

add_subdirectory(${spine_cpp_SOURCE_DIR}/spine-cpp)

file(GLOB_RECURSE SPINE_CPP_SOURCES
        "${spine_cpp_SOURCE_DIR}/spine-cpp/spine-cpp/src/*.cpp")