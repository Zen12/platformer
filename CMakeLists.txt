cmake_minimum_required(VERSION 3.11...3.30)
project(Engine-Demo)

include(FetchContent)

# Offline build support
# OFFLINE_BUILD=ON: Fully offline, fails if packages not cached
# OFFLINE_BUILD=OFF (default): Downloads missing packages, skips update checks for cached ones
option(OFFLINE_BUILD "Fully offline build (fails if dependencies not cached)" OFF)

if(OFFLINE_BUILD)
    message(STATUS "Offline build mode: FULLY_DISCONNECTED (will fail if dependencies not cached)")
    set(FETCHCONTENT_FULLY_DISCONNECTED ON)
else()
    message(STATUS "Online build mode: Will download missing dependencies, skip updates for cached ones")
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set minimum policy version for compatibility
cmake_policy(SET CMP0000 NEW)

# Auto-detect platform if not defined by build scripts
if(NOT IS_PLATFORM_DEFINED)
    message(STATUS "IS_PLATFORM_DEFINED not set, auto-detecting platform...")
    set(PLATFORM_DESKTOP ON)

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(PLATFORM_DESKTOP_OSX ON)
        message(STATUS "Auto-detected platform: macOS")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(PLATFORM_DESKTOP_LINUX ON)
        message(STATUS "Auto-detected platform: Linux")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(PLATFORM_DESKTOP_WINDOWS ON)
        message(STATUS "Auto-detected platform: Windows")
    else()
        message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    endif()
else()
    message(STATUS "Using platform settings from build script")
endif()

# Dependencies
find_package(YAMLCPP REQUIRED)
find_package(GLM REQUIRED)
find_package(EnTT REQUIRED)
find_package(RmlUi REQUIRED)
find_package(FREETYPE REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(RVO2 REQUIRED)
find_package(OpenAL REQUIRED)

if (PLATFORM_DESKTOP)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 3.3 REQUIRED)
endif()

if (VIDEO_RECORDING)
    find_package(FFMPEG REQUIRED)
endif()

# --- Source files ---
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# --- Create target ---
add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# --- Link libraries ---
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GLM::GLM freetype yaml-cpp EnTT::EnTT rmlui_core assimp RVO)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR} ${RVO2_INCLUDE_DIRS})

if (PLATFORM_DESKTOP)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw OpenAL::OpenAL)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${openal-soft_SOURCE_DIR}/include)
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

# Remove all _original folders from output (source files not needed at runtime)
execute_process(
    COMMAND find "${CMAKE_BINARY_DIR}/output/assets" -type d -name "_original"
    OUTPUT_VARIABLE ORIGINAL_DIRS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(ORIGINAL_DIRS)
    string(REPLACE "\n" ";" ORIGINAL_DIRS_LIST "${ORIGINAL_DIRS}")
    foreach(DIR ${ORIGINAL_DIRS_LIST})
        file(REMOVE_RECURSE "${DIR}")
    endforeach()
endif()

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

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "-lopenal -s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s STACK_SIZE=5MB -s INITIAL_MEMORY=256MB -s ASSERTIONS=1 -s EXCEPTION_DEBUG=1 -s NO_DISABLE_EXCEPTION_CATCHING -sLZ4 --shell-file ${CMAKE_SOURCE_DIR}/template/html/simple.html --preload-file ${CMAKE_BINARY_DIR}/output/assets@/assets --use-preload-cache --lz4")

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
