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
        const std::string name,
        const std::string guid,
        const std::string extention,
        const std::string type,
        const std::string path) : Name(name),
                                  Guid(guid),
                                  Extention(extention),
                                  Type(type),
                                  Path(path) {};

    AssetMeta() = default; // default constructor
};