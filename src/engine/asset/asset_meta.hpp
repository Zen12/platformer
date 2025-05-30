#include <iostream>
#include <yaml-cpp/yaml.h>

class AssetMeta
{
public:
    std::string Name;
    std::string Guid;
    std::string Extention;
    std::string Type;
    std::string Path;

    AssetMeta(
        const std::string &name,
        const std::string &guid,
        const std::string &extention,
        const std::string &type,
        const std::string &path) : Name(name),
                                   Guid(guid),
                                   Extention(extention),
                                   Type(type),
                                   Path(path) {};

    AssetMeta() = delete;
    AssetMeta(const AssetMeta &) = default;                // copy constructor
    AssetMeta(AssetMeta &&) noexcept = default;            // move constructor
    AssetMeta &operator=(const AssetMeta &) = default;     // copy assignment
    AssetMeta &operator=(AssetMeta &&) noexcept = default; // move assignment
};