# Engine GUID Module
# UUID generation and parsing utilities

add_library(engine-guid STATIC
    guid.cpp
    guid.hpp
    guid_yaml.hpp
    guid_generator.hpp
)

target_include_directories(engine-guid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# YAML-CPP dependency (from parent) for guid_yaml.hpp
target_link_libraries(engine-guid PUBLIC yaml-cpp)

# C++17 required
target_compile_features(engine-guid PUBLIC cxx_std_17)
