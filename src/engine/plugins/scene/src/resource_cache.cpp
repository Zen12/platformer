#include "resource_cache.hpp"
#include <iostream>

#define SHADER_LOG_ENABLED 0
#if SHADER_LOG_ENABLED
#define SHADER_LOG if(1) std::cout
#else
#define SHADER_LOG if(0) std::cout
#endif

#define MATERIAL_LOG_ENABLED 0
#if MATERIAL_LOG_ENABLED
#define MATERIAL_LOG if(1) std::cout
#else
#define MATERIAL_LOG if(0) std::cout
#endif

static const Guid SPRITE_GUID = Guid(0, 2);

std::shared_ptr<Shader> ResourceCache::GetShader(const Guid &vertexGuid, const Guid &fragmentGuid) {
    if (const auto assetManager = _assetManager.lock()) {
        Guid combinedGuid(vertexGuid.High() ^ fragmentGuid.High(), vertexGuid.Low() ^ fragmentGuid.Low());

        if (auto cached = _shaders.Get(combinedGuid)) {
            return cached;
        }

        SHADER_LOG << "Compiling shader: " << assetManager->GetPathFromGuid(vertexGuid)
                   << " + " << assetManager->GetPathFromGuid(fragmentGuid) << std::endl;
        const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(vertexGuid);
        const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(fragmentGuid);
        const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
        _shaders.Store(combinedGuid, shader);
        return shader;
    }
    return {};
}

std::shared_ptr<Material> ResourceCache::GetMaterial(const Guid &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (auto cached = _materials.Get(guid)) {
            return cached;
        }

        MATERIAL_LOG << "Loading material: " << assetManager->GetPathFromGuid(guid) << std::endl;
        const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
        const auto shader = GetShader(materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid);
        const auto material = std::make_shared<Material>(shader);

        const auto font = GetFont(materialAsset.Font);
        material->SetFont(font);
        material->SetBlendMode(static_cast<BlendMode>(materialAsset.BlendMode));
        material->SetCulling(materialAsset.IsCulling);
        material->SetDepthTest(materialAsset.IsDepthTest);
        material->SetDepthWrite(materialAsset.IsDepthWrite);

        if (!materialAsset.Image.IsEmpty()) {
            const auto sprite = GetTexture(materialAsset.Image);
            material->AddSprite(sprite);
        }

        _materials.Store(guid, material);
        return material;
    }

    return {};
}

std::shared_ptr<Mesh> ResourceCache::GetMesh(const Guid &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (auto cached = _meshes.Get(guid)) {
            return cached;
        }

        if (guid == SPRITE_GUID) {
            const auto meshAsset = std::shared_ptr<Mesh>(Mesh::GenerateSpritePtr());
            _meshes.Store(guid, meshAsset);
            return meshAsset;
        }

        const auto meshData = assetManager->LoadSourceByGuid<MeshData>(guid);
        std::shared_ptr<Mesh> mesh;
        if (meshData.HasSkeleton) {
            mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords,
                                          meshData.BoneWeights, meshData.BoneIndices);
            _meshBoneNames.Store(guid, meshData.BoneNames);
            _meshBoneOffsets.Store(guid, meshData.BoneOffsets);
            _meshBoneParents.Store(guid, meshData.BoneParents);
        } else {
            mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords);
        }
        _meshBounds.Store(guid, meshData.MeshBounds);
        _meshes.Store(guid, mesh);
        return mesh;
    }

    return {};
}

std::shared_ptr<Texture> ResourceCache::GetTexture(const Guid &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        return _textures.GetOrLoad(guid, [&]() {
            return std::make_shared<Texture>(assetManager->LoadSourceByGuid<Texture>(guid));
        });
    }
    return {};
}

std::shared_ptr<Font> ResourceCache::GetFont(const Guid &guid) {
    if (guid.IsEmpty())
        return {};
    if (const auto assetManager = _assetManager.lock()) {
        return _fonts.GetOrLoad(guid, [&]() {
            auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
            return std::make_shared<Font>(std::move(fontFile));
        });
    }
    return {};
}

std::shared_ptr<UiPage> ResourceCache::GetUiPage(const Guid &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (auto cached = _uiPages.Get(guid)) {
            return cached;
        }

        const auto uiPageAsset = assetManager->LoadAssetByGuid<UiPageAsset>(guid);

        const auto uiPage = std::make_shared<UiPage>();
        uiPage->Rml = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.RmlGuid);
        uiPage->Material = GetMaterial(uiPageAsset.MaterialGuid);
        uiPage->FontPath = assetManager->GetPathFromGuid(uiPageAsset.FontGuid);
        uiPage->Css = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.CssGuid);

        _uiPages.Store(guid, uiPage);
        return uiPage;
    }

    return {};
}

std::shared_ptr<AnimationData> ResourceCache::GetAnimation(const Guid &guid) {
    if (guid.IsEmpty())
        return {};
    if (const auto assetManager = _assetManager.lock()) {
        return _animations.GetOrLoad(guid, [&]() {
            auto animData = assetManager->LoadSourceByGuid<AnimationData>(guid);
            return std::make_shared<AnimationData>(std::move(animData));
        });
    }
    return {};
}

std::vector<std::string> ResourceCache::GetMeshBoneNames(const Guid &guid) const {
    return _meshBoneNames.Get(guid);
}

std::vector<glm::mat4> ResourceCache::GetMeshBoneOffsets(const Guid &guid) const noexcept {
    return _meshBoneOffsets.Get(guid);
}

std::vector<int> ResourceCache::GetMeshBoneParents(const Guid &guid) const noexcept {
    return _meshBoneParents.Get(guid);
}

Bounds ResourceCache::GetMeshBounds(const Guid &guid) const noexcept {
    return _meshBounds.Get(guid);
}
