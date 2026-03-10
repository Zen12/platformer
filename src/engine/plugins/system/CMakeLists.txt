# Engine System Module
# Window, input, and time management

add_library(engine-system STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-system PRIVATE ${SOURCES})

target_include_directories(engine-system PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine-system PUBLIC
    GLM::GLM
)

if(PLATFORM_DESKTOP)
    target_link_libraries(engine-system PUBLIC glfw GLEW::GLEW)
endif()
