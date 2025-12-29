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

if (PLATFORM_DESKTOP)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 3.3 REQUIRED)
endif()

if (VIDEO_RECORDING)
    find_package(FFMPEG REQUIRED)
endif()

# stb - header-only library
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# --- Source files ---
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# --- Create target ---
add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# --- Link libraries ---
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GLM::GLM freetype yaml-cpp EnTT::EnTT rmlui_core assimp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

if (PLATFORM_DESKTOP)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PLATFORM_DESKTOP)
endif()

if (VIDEO_RECORDING)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE FFMPEG::avcodec FFMPEG::avformat FFMPEG::avutil FFMPEG::swscale)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE VIDEO_RECORDING)
endif()

if (PLATFORM_WEB)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PLATFORM_WEB)
endif()

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/output)
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}/output")

# --- Compiler flags (desktop only) ---
if (PLATFORM_DESKTOP)
    if(PLATFORM_DESKTOP_WINDOWS) # MSVC
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
endif()

# --- Platform-specific configuration ---
if (PLATFORM_WEB)

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "index"
            SUFFIX ".html"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
    )

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s STACK_SIZE=5MB -s INITIAL_MEMORY=256MB -s ASSERTIONS=1 -s EXCEPTION_DEBUG=1 -s NO_DISABLE_EXCEPTION_CATCHING --shell-file ${CMAKE_SOURCE_DIR}/template/html/simple.html --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")

endif()

if (PLATFORM_DESKTOP)
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
    )

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
endif()
