#include "scene.hpp"
#include "../renderer/material/material_asset.hpp"
#include "../renderer/material/material_asset_yaml.hpp"


std::weak_ptr<AssetManager> Scene::GetAssetManager() const noexcept {
    return _assetManager;
}

std::weak_ptr<InputSystem> Scene::GetInputSystem() const noexcept {
    return _inputSystem;
}

std::weak_ptr<Window> Scene::GetWindow() const noexcept {
    return _window;
}


std::shared_ptr<Shader> Scene::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) {
    if (const auto assetManager = _assetManager.lock()) {

        if (_shaders.find(vertexGuid + fragmentGuid) == _shaders.end()) {
            const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(vertexGuid);
            const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(fragmentGuid);
            const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
            _shaders[vertexGuid + fragmentGuid] = shader;
            return shader;
        }

        return _shaders[vertexGuid + fragmentGuid];
    }

    return {};
}

std::shared_ptr<Material> Scene::GetMaterial(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_materials.find(guid) == _materials.end()) {
            const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
            const auto shader = GetShader(materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid);
            const auto material = std::make_shared<Material>(shader);

            const auto font = GetFont(materialAsset.Font);
            material->SetFont(font);
            material->SetBlendMode(static_cast<BlendMode>(materialAsset.BlendMode));
            material->SetCulling(materialAsset.IsCulling);

            if (!materialAsset.Image.empty()) {
                const auto sprite = GetTexture(materialAsset.Image);
                material->AddSprite(sprite);
            }

            _materials[guid] = material;
            return material;
        }
        return _materials[guid];
    }

    return {};
}

std::shared_ptr<Mesh> Scene::GetMesh(const std::string &guid) {

    if (const auto assetManager = _assetManager.lock()) {
        if (_meshes.find(guid) == _meshes.end()) {
            const auto meshAsset = std::shared_ptr<Mesh>(Mesh::GenerateSpritePtr());
            _meshes[guid] = meshAsset;
            return meshAsset;
        }
        return _meshes[guid];
    }

    return {};
}

std::shared_ptr<Texture> Scene::GetTexture(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_sprites.find(guid) == _sprites.end()) {
            const auto sprite = std::make_shared<Texture>(assetManager->LoadSourceByGuid<Texture>(guid));
            _sprites[guid] = sprite;
            return sprite;
        }
        return _sprites[guid];
    }
    return {};
}

std::shared_ptr<Font> Scene::GetFont(const std::string &guid) {
    if (guid.empty())
        return {};

    if (const auto assetManager = _assetManager.lock()) {
        if (_fonts.find(guid) == _fonts.end()) {
            auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
            const auto font = std::make_shared<Font>(std::move(fontFile));
            _fonts[guid] = font;
            return font;
        }
        return _fonts[guid];
    }
    return {};
}

