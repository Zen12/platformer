cmake_minimum_required(VERSION 3.11...3.30)
project(Engine-Demo)

option(ENGINE_BUILD_DEMO "Build the platformer demo game and executable" ON)

include(FetchContent)

# Testing option (must be before include(CTest))
option(BUILD_TESTING "Build unit tests" OFF)
include(CTest)

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

# Save BUILD_TESTING before RmlUi (which forces it OFF)
set(_SAVED_BUILD_TESTING ${BUILD_TESTING})
find_package(RmlUi REQUIRED)
set(BUILD_TESTING ${_SAVED_BUILD_TESTING} CACHE BOOL "" FORCE)

find_package(FREETYPE REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(RVO2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Minitrace REQUIRED)
find_package(XSIMD REQUIRED)
find_package(NlohmannJson REQUIRED)
find_package(Manifold REQUIRED)

if (PLATFORM_DESKTOP)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 3.3 REQUIRED)
endif()

if (VIDEO_RECORDING)
    find_package(FFMPEG REQUIRED)
endif()

# Testing framework
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    enable_testing()
endif()

# --- Engine Modules (order matters for dependencies) ---
add_subdirectory(src/engine/plugins/profiler)
add_subdirectory(src/engine/plugins/math)
add_subdirectory(src/engine/plugins/utils)
add_subdirectory(src/engine/plugins/system)
add_subdirectory(src/engine/core)
add_subdirectory(src/engine/plugins/font)
add_subdirectory(src/engine/plugins/prefab)
add_subdirectory(src/engine/plugins/project)
add_subdirectory(src/engine/plugins/navigation)
add_subdirectory(src/engine/plugins/audio)
add_subdirectory(src/engine/plugins/animation)
add_subdirectory(src/engine/plugins/ik)
add_subdirectory(src/engine/plugins/ai)
add_subdirectory(src/engine/plugins/renderer)
add_subdirectory(src/engine/plugins/bezier)
add_subdirectory(src/engine/plugins/mesh_generator)
add_subdirectory(src/engine/plugins/scene)

# Terminal and MCP server (desktop debug only)
if(PLATFORM_DESKTOP AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(src/engine/plugins/terminal)
    add_subdirectory(src/engine/plugins/mcp)
endif()

# Examples
add_subdirectory(src/engine/examples/plugin_example)

# --- Umbrella target for consumers ---
add_library(engine-all INTERFACE)
target_link_libraries(engine-all INTERFACE
    engine-profiler
    engine-math
    engine-utils
    engine-system
    engine-core
    engine-font
    engine-prefab
    engine-project
    engine-navigation
    engine-audio
    engine-ai
    engine-animation
    engine-ik
    engine-renderer
    engine-bezier
    engine-mesh-generator
    engine-scene
)

if(ENGINE_BUILD_DEMO)

if(NOT DEFINED ENGINE_DEMO_ASSETS_DIR)
    set(ENGINE_DEMO_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()

# Game
add_subdirectory(src/game)

# --- Source files ---
file(GLOB_RECURSE HEADER_FILES "src/path_utils.hpp" "src/engine/engine.hpp")
file(GLOB_RECURSE SOURCE_FILES "src/main.cpp" "src/engine/engine.cpp")


# --- Application icon ---
set(APP_ICON_SOURCES "")
if(PLATFORM_DESKTOP_OSX)
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(APP_ICON_MACOSX "${CMAKE_SOURCE_DIR}/template/macos/icon.icns")
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(APP_ICON_SOURCES ${APP_ICON_MACOSX})
elseif(PLATFORM_DESKTOP_WINDOWS)
    set(APP_ICON_SOURCES "${CMAKE_SOURCE_DIR}/template/windows/icon.rc")
endif()

# --- Create target ---
if(PLATFORM_DESKTOP_OSX)
    add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${HEADER_FILES} ${SOURCE_FILES} ${APP_ICON_SOURCES})
else()
    add_executable(${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${APP_ICON_SOURCES})
endif()

# --- Link libraries ---
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GLM::GLM freetype yaml-cpp EnTT::EnTT rmlui_core assimp RVO xsimd)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${stb_SOURCE_DIR}
    ${RVO2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/scene/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/renderer/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/navigation/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/animation/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/audio/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/ai/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/ik/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/prefab/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/profiler/src
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/bezier
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/mesh_generator
    ${CMAKE_SOURCE_DIR}/src/engine
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/terminal
    ${CMAKE_SOURCE_DIR}/src/engine/plugins/mcp
)

# Engine modules
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    engine-all
    engine-game
    engine-example-plugin
    $<$<AND:$<BOOL:${PLATFORM_DESKTOP}>,$<CONFIG:Debug>>:engine-terminal>
    $<$<AND:$<BOOL:${PLATFORM_DESKTOP}>,$<CONFIG:Debug>>:engine-mcp>
)

if (PLATFORM_DESKTOP)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw OpenAL::OpenAL)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${openal-soft_SOURCE_DIR}/include)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PLATFORM_DESKTOP)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DEBUG_BUILD)
    endif()
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

    # macOS app bundle with icon (for release/distribution)
    if(PLATFORM_DESKTOP_OSX)
        set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE icon.icns
            MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}"
        )

        # Move assets into the .app bundle after build
        set(BUNDLE_RESOURCES_DIR "${CMAKE_BINARY_DIR}/output/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_BINARY_DIR}/output/assets"
                "${BUNDLE_RESOURCES_DIR}/assets"
            COMMAND ${CMAKE_COMMAND} -E rm -rf
                "${CMAKE_BINARY_DIR}/output/assets"
            COMMENT "Moving assets into macOS app bundle"
        )
    endif()

    if(PLATFORM_DESKTOP_OSX)
        # macOS: executable is inside .app bundle
        set(APP_EXECUTABLE "${CMAKE_BINARY_DIR}/output/${CMAKE_PROJECT_NAME}.app/Contents/MacOS/${CMAKE_PROJECT_NAME}")
    else()
        set(APP_EXECUTABLE "${CMAKE_BINARY_DIR}/output/${CMAKE_PROJECT_NAME}")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_target(run
                COMMAND "${APP_EXECUTABLE}" "${ENGINE_DEMO_ASSETS_DIR}"
                DEPENDS ${CMAKE_PROJECT_NAME}
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/output"
        )
    else()
        add_custom_target(run
                COMMAND "${APP_EXECUTABLE}"
                DEPENDS ${CMAKE_PROJECT_NAME}
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/output"
        )
    endif()
endif()

endif() # ENGINE_BUILD_DEMO
