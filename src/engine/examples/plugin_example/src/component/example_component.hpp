#pragma once
#include <string>

// Example ECS component that stores a tag string
// Usage in .scene/.prefab: type: example_tag
struct ExampleTagComponent {
    std::string Tag;
};
