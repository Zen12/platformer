#include "spine_atlas_loader.hpp"

// Static member initialization
std::function<std::shared_ptr<AssetTextureLoader>()> AssetLoader<spine::Atlas*>::_textureLoader = nullptr;

void AssetLoader<spine::Atlas*>::Init() {
    AssetLoader::_textureLoader = []() {
        return std::make_shared<AssetTextureLoader>();
    };
}

spine::Atlas* AssetLoader<spine::Atlas*>::LoadImpl(const std::string& path) {
    const auto loader = _textureLoader();

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open atlas file: " + path);
    }
    std::string buffer((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    std::string dir;
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dir = path.substr(0, lastSlash);
    } else {
        dir = ".";
    }

    auto* atlas = new spine::Atlas(buffer.data(), static_cast<int>(buffer.size()), dir.c_str(), loader.get(), true);

    return atlas;
}
