#include "asset_manager.hpp"

namespace fs = std::filesystem;

const std::string metaExt = ".meta";

void AssetManager::Init()
{

    for (const auto &entry : fs::recursive_directory_iterator(_assetPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == metaExt)
        {
            try
            {
                // move to a serializer?
                YAML::Node root = YAML::LoadFile(entry.path().string());
                if (root["guid"])
                {
                    // Extract values safely with .as<>()
                    const std::string name = root["name"] ? root["name"].as<std::string>() : "";
                    const std::string guid = root["guid"].as<std::string>(); // Already checked
                    const std::string extension = root["extention"] ? root["extention"].as<std::string>() : "";
                    const std::string type = root["type"] ? root["type"].as<std::string>() : "";

                    auto newPath = entry.path();
                    newPath.replace_extension();
                    _assetMap[guid] = AssetMeta(name, guid, extension, type, newPath.c_str());
                }
            }
            catch (const YAML::Exception &e)
            {
                std::cerr << "YAML parse error in " << entry.path() << ": " << e.what() << "\n";
            }
        }
    }
}

void AssetManager::LoadSceneByGuid(const std::string &guid)
{
    const auto meta = _assetMap[guid];
}

void AssetManager::UnLoad(const std::string &guid)
{
    std::cout << guid << "\n";
}