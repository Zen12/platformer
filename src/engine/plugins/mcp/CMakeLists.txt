add_library(engine-mcp STATIC)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

target_sources(engine-mcp PRIVATE ${SOURCES})

target_include_directories(engine-mcp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(engine-mcp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(engine-mcp PUBLIC
    engine-core
    nlohmann_json::nlohmann_json
)
