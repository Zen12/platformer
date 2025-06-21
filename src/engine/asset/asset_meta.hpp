#include <iostream>
#include <utility>
#include <yaml-cpp/yaml.h>

class AssetMeta
{
public:
    std::string Name;
    std::string Guid;
    std::string Extension;
    std::string Type;
    std::string Path;

    AssetMeta(
        std::string name,
        std::string guid,
        std::string extension,
        std::string type,
        std::string path) : Name(std::move(name)),
                                   Guid(std::move(guid)),
                                   Extension(std::move(extension)),
                                   Type(std::move(type)),
                                   Path(std::move(path)) {}

    AssetMeta() = delete;
    AssetMeta(const AssetMeta &) = default;                // copy constructor
    AssetMeta(AssetMeta &&) noexcept = default;            // move constructor
    AssetMeta &operator=(const AssetMeta &) = default;     // copy assignment
    AssetMeta &operator=(AssetMeta &&) noexcept = default; // move assignment
};