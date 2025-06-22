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
                    const auto guid = root["guid"].as<std::string>(); // Already checked
                    const std::string extension = root["extention"] ? root["extention"].as<std::string>() : "";
                    const std::string type = root["type"] ? root["type"].as<std::string>() : "";

                    auto newPath = entry.path();
                    newPath.replace_extension();

                    const std::string path = newPath.string();

                    _assetMap.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(guid),
                        std::forward_as_tuple(name, guid, extension, type, path));
                }
                else
                {
                    std::cerr << "NO GUID!\n";
                }
            }
            catch (const YAML::Exception &e)
            {
                std::cerr << "YAML parse error in " << entry.path() << ": " << e.what() << "\n";
            }
        }
    }
}

SceneSerialization AssetManager::LoadSceneByGuid(const std::string &guid) const {
    const auto meta = _assetMap.at(guid);
    const YAML::Node root = YAML::LoadFile(meta.Path);

    return root.as<SceneSerialization>();
}

ShaderComponentSerialization AssetManager::LoadShaderByGuid(const std::string &vertexGuid, const std::string &fragmentGuid) const {
    const auto vertexMeta = _assetMap.at(vertexGuid);
    const auto fragMeta = _assetMap.at(fragmentGuid);

    const std::string vertexShaderSource = FileLoader::LoadFile(vertexMeta.Path);
    const std::string fragmentShaderSource = FileLoader::LoadFile(fragMeta.Path);

    return {vertexShaderSource, fragmentShaderSource};
};

MaterialComponentSerialization AssetManager::LoadMaterialByGuid(const std::string &guid) const {
    const auto meta = _assetMap.at(guid);
    const YAML::Node root = YAML::LoadFile(meta.Path);
    const YAML::Node shaderNode = root["shader"];

    const auto shader = LoadShaderByGuid(shaderNode["vertex"].as<std::string>(), shaderNode["fragment"].as<std::string>());

    return MaterialComponentSerialization{shader};
}

SpriteComponentSerialization AssetManager::LoadSpriteByGuid(const std::string &guid) const {
    const auto meta = _assetMap.at(guid);

    return SpriteComponentSerialization(meta.Path);
}

void AssetManager::UnLoad(const std::string &guid) const {
    std::cout << guid << "\n";
}