#pragma once
#include <string>
#include <vector>

// Example asset data structure loaded from a custom file format
// Demonstrates how plugins can define their own asset types
struct ExampleAsset {
    std::string Name;
    std::vector<std::string> Entries;
};
