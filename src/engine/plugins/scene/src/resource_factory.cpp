#include "resource_factory.hpp"
#include "material/shader.hpp"
#include "material/material.hpp"
#include "mesh/mesh.hpp"
#include "texture/texture_asset_loader.hpp"
#include "mesh/mesh_asset_loader.hpp"
#include "font_loader.hpp"
#include "ui/ui_page.hpp"
#include "material/material_asset.hpp"
#include "material/material_asset_yaml.hpp"
#include "ui/ui_page_asset.hpp"
#include "ui/ui_page_asset_yaml.hpp"
#include "animation/animation_data.hpp"
#include "animation/animation_asset_loader.hpp"
#include "core/src/fsm/fsm_asset.hpp"
#include "core/src/fsm/fsm_asset_yaml.hpp"
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

template <>
std::shared_ptr<Shader> ResourceFactory::Get<Shader>(const std::tuple<Guid, Guid>& key) {
    if (const auto assetManager = _assetManager.lock()) {
        const auto& [vertexGuid, fragmentGuid] = key;
        Guid combinedGuid(vertexGuid.High() ^ fragmentGuid.High(), vertexGuid.Low() ^ fragmentGuid.Low());

        if (auto cached = _cache->Get<Shader>(combinedGuid)) {
            return cached;
        }

        SHADER_LOG << "Compiling shader: " << assetManager->GetPathFromGuid(vertexGuid)
                   << " + " << assetManager->GetPathFromGuid(fragmentGuid) << std::endl;
        const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(vertexGuid);
        const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(fragmentGuid);
        const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
        _cache->Register(combinedGuid, shader);
        return shader;
    }
    return {};
}

template <>
std::shared_ptr<Material> ResourceFactory::Get<Material>(const Guid& guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (auto cached = _cache->Get<Material>(guid)) {
            return cached;
        }

        MATERIAL_LOG << "Loading material: " << assetManager->GetPathFromGuid(guid) << std::endl;
        const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
        const auto shader = Get<Shader>({materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid});
        const auto material = std::make_shared<Material>(shader);

        const auto font = Get<Font>(materialAsset.Font);
        material->SetFont(font);
        material->SetBlendMode(static_cast<BlendMode>(materialAsset.BlendMode));
        material->SetCulling(materialAsset.IsCulling);
        material->SetDepthTest(materialAsset.IsDepthTest);
        material->SetDepthWrite(materialAsset.IsDepthWrite);

        if (!materialAsset.Image.IsEmpty()) {
            const auto sprite = Get<Texture>(materialAsset.Image);
            material->AddSprite(sprite);
        }

        _cache->Register(guid, material);
        return material;
    }

    return {};
}

template <>
std::shared_ptr<Mesh> ResourceFactory::Get<Mesh>(const Guid& guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (auto cached = _cache->Get<Mesh>(guid)) {
            return cached;
        }

        if (guid == SPRITE_GUID) {
            const auto meshAsset = std::shared_ptr<Mesh>(Mesh::GenerateSpritePtr());
            _cache->Register(guid, meshAsset);
            return meshAsset;
        }

        const auto meshData = assetManager->LoadSourceByGuid<MeshData>(guid);
        std::shared_ptr<Mesh> mesh;
        if (meshData.HasSkeleton) {
            mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords,
                                          meshData.BoneWeights, meshData.BoneIndices);
            _cache->RegisterValue(guid, meshData.BoneNames);
            _cache->RegisterValue(guid, meshData.BoneOffsets);
            _cache->RegisterValue(guid, meshData.BoneParents);
        } else {
            mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords);
        }
        _cache->RegisterValue(guid, meshData.MeshBounds);
        _cache->Register(guid, mesh);
        return mesh;
    }

    return {};
}

template <>
std::shared_ptr<Texture> ResourceFactory::Get<Texture>(const Guid& guid) {
    if (const auto assetManager = _assetManager.lock()) {
        return _cache->GetOrLoad<Texture>(guid, [&]() {
            return std::make_shared<Texture>(assetManager->LoadSourceByGuid<Texture>(guid));
        });
    }
    return {};
}

template <>
std::shared_ptr<Font> ResourceFactory::Get<Font>(const Guid& guid) {
    if (guid.IsEmpty())
        return {};
    if (const auto assetManager = _assetManager.lock()) {
        return _cache->GetOrLoad<Font>(guid, [&]() {
            auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
            return std::make_shared<Font>(std::move(fontFile));
        });
    }
    return {};
}

template <>
std::shared_ptr<UiPage> ResourceFactory::Get<UiPage>(const Guid& guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (auto cached = _cache->Get<UiPage>(guid)) {
            return cached;
        }

        const auto uiPageAsset = assetManager->LoadAssetByGuid<UiPageAsset>(guid);

        const auto uiPage = std::make_shared<UiPage>();
        uiPage->Rml = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.RmlGuid);
        uiPage->Material = Get<Material>(uiPageAsset.MaterialGuid);
        uiPage->FontPath = assetManager->GetPathFromGuid(uiPageAsset.FontGuid);
        uiPage->Css = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.CssGuid);

        _cache->Register(guid, uiPage);
        return uiPage;
    }

    return {};
}

template <>
std::shared_ptr<AnimationData> ResourceFactory::Get<AnimationData>(const Guid& guid) {
    if (guid.IsEmpty())
        return {};
    if (const auto assetManager = _assetManager.lock()) {
        return _cache->GetOrLoad<AnimationData>(guid, [&]() {
            auto animData = assetManager->LoadSourceByGuid<AnimationData>(guid);
            return std::make_shared<AnimationData>(std::move(animData));
        });
    }
    return {};
}

template <>
std::shared_ptr<FsmAsset> ResourceFactory::Get<FsmAsset>(const Guid& guid) {
    if (guid.IsEmpty())
        return {};
    if (const auto assetManager = _assetManager.lock()) {
        return _cache->GetOrLoad<FsmAsset>(guid, [&]() {
            return std::make_shared<FsmAsset>(assetManager->LoadAssetByGuid<FsmAsset>(guid));
        });
    }
    return {};
}
