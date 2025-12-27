#include "scene.hpp"
#include "../renderer/ozz/ozz_skeleton_loader.hpp"
#include "../renderer/ozz/ozz_animation_loader.hpp"
#include <iostream>

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

std::shared_ptr<UiPage> Scene::GetUiPage(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_uiPages.find(guid) == _uiPages.end()) {
            const auto uiPageAsset = assetManager->LoadAssetByGuid<UiPageAsset>(guid);

            const auto uiPage = std::make_shared<UiPage>();
            uiPage->Rml = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.RmlGuid);
            uiPage->Material = GetMaterial(uiPageAsset.MaterialGuid);
            uiPage->FontPath = assetManager->GetPathFromGuid(uiPageAsset.FontGuid);
            uiPage->Css = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.CssGuid);

            _uiPages[guid] = uiPage;
            return uiPage;
        }
        return _uiPages[guid];
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
    std::cout << "[SCENE] GetMesh called for GUID: " << guid << std::endl;
    if (const auto assetManager = _assetManager.lock()) {
        if (_meshes.find(guid) == _meshes.end()) {
            std::cout << "[SCENE] Mesh not cached, loading from asset manager..." << std::endl;
            if (guid == "sprite") {
                std::cout << "[SCENE] Generating sprite mesh" << std::endl;
                const auto meshAsset = std::shared_ptr<Mesh>(Mesh::GenerateSpritePtr());
                _meshes[guid] = meshAsset;
                return meshAsset;
            }

            const auto meshData = assetManager->LoadSourceByGuid<MeshData>(guid);
            std::cout << "[SCENE] Loaded mesh data - HasSkeleton: " << (meshData.HasSkeleton ? "yes" : "no") << std::endl;
            std::shared_ptr<Mesh> mesh;
            if (meshData.HasSkeleton) {
                std::cout << "[SCENE] Creating skinned mesh with " << meshData.BoneNames.size() << " bones" << std::endl;
                mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords,
                                               meshData.BoneWeights, meshData.BoneIndices);
                // Store bone names, offsets, and hierarchy for animation mapping
                _meshBoneNames[guid] = meshData.BoneNames;
                _meshBoneOffsets[guid] = meshData.BoneOffsets;
                _meshBoneParents[guid] = meshData.BoneParents;
                std::cout << "[SCENE] Stored bone data for GUID: " << guid << std::endl;
            } else {
                std::cout << "[SCENE] Creating standard mesh (no skeleton)" << std::endl;
                mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords);
            }
            _meshes[guid] = mesh;
            std::cout << "[SCENE] Mesh cached successfully" << std::endl;
            return mesh;
        }
        std::cout << "[SCENE] Returning cached mesh" << std::endl;
        return _meshes[guid];
    }

    std::cerr << "[SCENE] ERROR: AssetManager lock failed for GUID: " << guid << std::endl;
    return {};
}

std::shared_ptr<Texture> Scene::GetTexture(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_textures.find(guid) == _textures.end()) {
            const auto sprite = std::make_shared<Texture>(assetManager->LoadSourceByGuid<Texture>(guid));
            _textures[guid] = sprite;
            return sprite;
        }
        return _textures[guid];
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

std::shared_ptr<AnimationData> Scene::GetAnimation(const std::string &guid) {
    if (guid.empty())
        return {};

    if (const auto assetManager = _assetManager.lock()) {
        if (_animations.find(guid) == _animations.end()) {
            auto animData = assetManager->LoadSourceByGuid<AnimationData>(guid);
            const auto animation = std::make_shared<AnimationData>(std::move(animData));
            _animations[guid] = animation;
            return animation;
        }
        return _animations[guid];
    }
    return {};
}



// Ozz animation getters
std::shared_ptr<ozz::animation::Skeleton> Scene::GetOzzSkeleton(const std::string &guid) {
    if (guid.empty())
        return {};

    // Check cache first
    if (_ozzSkeletons.find(guid) != _ozzSkeletons.end()) {
        return _ozzSkeletons[guid];
    }

    // Load from asset manager
    if (const auto assetManager = _assetManager.lock()) {
        try {
            // For now, treat guid as direct file path or resolve via asset manager
            // This will load the skeleton from the mesh file (e.g., shooter.glb)
            std::string assetPath = assetManager->GetPathFromGuid(guid);
            if (assetPath.empty()) {
                std::cerr << "[SCENE] Failed to resolve GUID to path: " << guid << std::endl;
                return {};
            }

            auto skeleton = OzzSkeletonLoader::Load(assetPath);
            if (skeleton) {
                _ozzSkeletons[guid] = skeleton;
                return skeleton;
            }
        } catch (const std::exception& e) {
            std::cerr << "[SCENE] Error loading ozz skeleton: " << e.what() << std::endl;
        }
    }
    return {};
}

std::shared_ptr<ozz::animation::Animation> Scene::GetOzzAnimation(const std::string &guid) {
    if (guid.empty())
        return {};

    // Check cache first
    if (_ozzAnimations.find(guid) != _ozzAnimations.end()) {
        return _ozzAnimations[guid];
    }

    // Load from asset manager
    if (const auto assetManager = _assetManager.lock()) {
        try {
            std::string assetPath = assetManager->GetPathFromGuid(guid);
            if (assetPath.empty()) {
                std::cerr << "[SCENE] Failed to resolve GUID to path: " << guid << std::endl;
                return {};
            }

            // Need skeleton first for animation loading
            // For now, assume animation GUID points to same file as skeleton
            // In production, you might have separate animation files
            auto skeleton = GetOzzSkeleton(guid); // Use same GUID for skeleton
            if (!skeleton) {
                std::cerr << "[SCENE] Cannot load animation without skeleton" << std::endl;
                return {};
            }

            auto animation = OzzAnimationLoader::Load(assetPath, *skeleton);
            if (animation) {
                _ozzAnimations[guid] = animation;
                return animation;
            }
        } catch (const std::exception& e) {
            std::cerr << "[SCENE] Error loading ozz animation: " << e.what() << std::endl;
        }
    }
    return {};
}
