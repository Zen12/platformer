#include "asset_manager.h"

namespace fs = std::filesystem;

const std::string metaExt = ".meta";


void AssetManager::Load()
{

    for (const auto &entry : fs::recursive_directory_iterator(_assetPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == metaExt)
        {
            try
            {
                YAML::Node root = YAML::LoadFile(entry.path().string());
                if (root["guid"])
                {
                    // Extract values safely with .as<>()
                    std::string name = root["name"] ? root["name"].as<std::string>() : "";
                    std::string guid = root["guid"].as<std::string>(); // Already checked
                    std::string extension = root["extention"] ? root["extention"].as<std::string>() : "";
                    std::string type = root["type"] ? root["type"].as<std::string>() : "";
                    
                
                    auto newPath = entry.path();
                    newPath.replace_extension();
                    AssetMeta meta(name, guid, extension, type, newPath.c_str());

                    _assetMap[guid] = meta;
                }
            }
            catch (const YAML::Exception &e)
            {
                std::cerr << "YAML parse error in " << entry.path() << ": " << e.what() << "\n";
            }
        }
    }
}

AssetMeta AssetManager::GetAssetMetaByGuid(const std::string guid) const
{
    return _assetMap.at(guid);
}

AssetMeta AssetManager::GetAssetProjectMeta() const
{
    for (auto const [guid, meta] : _assetMap)
    {
        if (meta.Type == "project")
            return meta;
    }
    
    return AssetMeta();
}
