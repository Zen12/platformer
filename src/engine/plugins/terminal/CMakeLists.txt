# Engine Terminal Module
# Embedded shell terminal (desktop only)

# --- Vendored libvterm (C library) ---
set(LIBVTERM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libvterm)

add_library(libvterm STATIC
    ${LIBVTERM_DIR}/src/encoding.c
    ${LIBVTERM_DIR}/src/keyboard.c
    ${LIBVTERM_DIR}/src/mouse.c
    ${LIBVTERM_DIR}/src/parser.c
    ${LIBVTERM_DIR}/src/pen.c
    ${LIBVTERM_DIR}/src/screen.c
    ${LIBVTERM_DIR}/src/state.c
    ${LIBVTERM_DIR}/src/unicode.c
    ${LIBVTERM_DIR}/src/vterm.c
)

target_include_directories(libvterm PUBLIC
    ${LIBVTERM_DIR}/include
)

target_include_directories(libvterm PRIVATE
    ${LIBVTERM_DIR}/src
    ${LIBVTERM_DIR}/include
)

# Suppress warnings in vendored C code
target_compile_options(libvterm PRIVATE
    -w
)

# --- Engine Terminal Plugin ---
add_library(engine-terminal STATIC)

file(GLOB TERMINAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_emulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pty_process_posix.cpp
)

file(GLOB TERMINAL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-terminal PRIVATE ${TERMINAL_SOURCES} ${TERMINAL_HEADERS})
target_sources(engine-terminal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/terminal_plugin.hpp)

target_include_directories(engine-terminal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Cross-module includes
target_include_directories(engine-terminal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(engine-terminal PUBLIC
    engine-core
    engine-font
    engine-system
    libvterm
    GLM::GLM
    freetype
    EnTT::EnTT
)

if(PLATFORM_DESKTOP)
    target_link_libraries(engine-terminal PRIVATE GLEW::GLEW OpenGL::GL glfw)
endif()

# Platform-specific PTY linkage
if(PLATFORM_DESKTOP_LINUX)
    target_link_libraries(engine-terminal PRIVATE util)
endif()
