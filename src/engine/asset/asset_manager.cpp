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

void AssetManager::LoadSceneByGuid(const std::string &guid)
{
    auto meta = _assetMap.at(guid);
    YAML::Node root = YAML::LoadFile(meta.Path);

    SceneSerialization scene = root.as<SceneSerialization>();

    try
    {
        YAML::Node root = YAML::LoadFile(meta.Path);

        SceneSerialization scene = root.as<SceneSerialization>();

        std::cout << "Scene: " << scene.name << " (" << scene.type << ")\n";
        std::cout << "Entities size: " << scene.entities.size() << "\n";
        for (const auto &entity : scene.entities)
        {
            std::cout << "- Entity: " << entity.guid << "\n";
            for (const auto &comp : entity.components)
            {
                std::cout << "  * Component: " << comp->getType() << "\n";

                if (comp->getType() == "camera")
                {
                    auto *cam = dynamic_cast<CameraComponentSerialization *>(comp.get());
                    std::cout << "      aspectPower: " << cam->aspectPower
                              << ", isPerspective: " << cam->isPerspective << "\n";
                }
                else if (comp->getType() == "tranform")
                {
                    auto *tf = dynamic_cast<TransformComponentSerialization *>(comp.get());
                    std::cout << "      position: (" << tf->position.x << ", "
                              << tf->position.y << ", " << tf->position.z << ")\n";
                }
            }
        }
        std::cout << "Hierarchy size: " << scene.hierarchy.size() << "\n";

        for (const auto &obj : scene.hierarchy)
        {
            std::cout << "  * Object: " << obj.guid << "\n";
            for (const auto &child : obj.children)
            {
                std::cout << "      * Child: " << child << "\n";
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void AssetManager::UnLoad(const std::string &guid)
{
    std::cout << guid << "\n";
}