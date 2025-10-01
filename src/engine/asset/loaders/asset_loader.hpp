#pragma once

#include <fstream>
#include <memory>
#include <type_traits>

#include <yaml-cpp/yaml.h>
#include <iostream>

// CRTP Base class
template<typename T, typename Derived>
class AssetLoaderBase {
public:
    static T LoadFromPath(const std::string& path) {
        return Derived::LoadImpl(path);
    }
};

// Default loader - triggers compile error for unsupported types
template<typename T>
class AssetLoader : public AssetLoaderBase<T, AssetLoader<T>> {
public:
    static T LoadImpl([[maybe_unused]] const std::string& path) {
        static_assert(sizeof(T) == 0, "AssetLoader::LoadFromPath<T> not implemented for this type");
        return T{};
    }
};

// Specialization for std::string (file contents)
template<>
class AssetLoader<std::string> : public AssetLoaderBase<std::string, AssetLoader<std::string>> {
public:
    static std::string LoadImpl(const std::string& path) {
        std::ifstream file(path.c_str());

        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file at: " << path << "\n";
            return {};
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string content = buffer.str();
        file.close();

        return content;
    }
};

// Type trait for compile-time checks
namespace detail {
    template<typename T, typename = void>
    struct HasAssetLoader : std::false_type {};

    template<typename T>
    struct HasAssetLoader<T, std::void_t<decltype(AssetLoader<T>::LoadImpl(std::declval<std::string>()))>>
        : std::true_type {};
}

template<typename T>
constexpr bool has_asset_loader_v = detail::HasAssetLoader<T>::value;