#include "asset_manager.hpp"

namespace fs = std::filesystem;

const std::string metaExt = ".meta";

void AssetManager::Init()
{
    for (const auto &entry : fs::recursive_directory_iterator(_assetPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == metaExt)
        {
            auto meta = YAML::LoadFile(entry.path().string()).as<MetaAsset>();

            auto newPath = entry.path();
            newPath.replace_extension();

            meta.Path = newPath.string();
            _assetMap.insert(std::make_pair(meta.Guid, meta));
        }
    }
}
