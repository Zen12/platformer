#pragma once
#include "resource_factory.hpp"
#include "material/shader.hpp"
#include "material/material.hpp"
#include "material/material_asset.hpp"
#include "material/material_asset_yaml.hpp"
#include "material/shader_resource_key.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_asset_loader.hpp"
#include "texture/texture_asset_loader.hpp"
#include "font_loader.hpp"
#include "ui/ui_page.hpp"
#include "ui/ui_page_asset.hpp"
#include "ui/ui_page_asset_yaml.hpp"
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

inline void RegisterRendererResources(ResourceFactory& resources) {
    // Shader
    resources.RegisterLoader<Shader>([](ResourceFactory& factory, const std::tuple<Guid, Guid>& key) -> std::shared_ptr<Shader> {
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            const auto& [vertexGuid, fragmentGuid] = key;
            Guid combinedGuid(vertexGuid.High() ^ fragmentGuid.High(), vertexGuid.Low() ^ fragmentGuid.Low());

            if (auto cached = factory.GetCache()->Get<Shader>(combinedGuid)) {
                return cached;
            }

            SHADER_LOG << "Compiling shader: " << assetManager->GetPathFromGuid(vertexGuid)
                       << " + " << assetManager->GetPathFromGuid(fragmentGuid) << std::endl;
            const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(vertexGuid);
            const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(fragmentGuid);
            const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
            factory.GetCache()->Register(combinedGuid, shader);
            return shader;
        }
        return {};
    });

    // Material
    resources.RegisterLoader<Material>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<Material> {
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            if (auto cached = factory.GetCache()->Get<Material>(guid)) {
                return cached;
            }

            MATERIAL_LOG << "Loading material: " << assetManager->GetPathFromGuid(guid) << std::endl;
            const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
            const auto shader = factory.Get<Shader>({materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid});
            const auto material = std::make_shared<Material>(shader);

            const auto font = factory.Get<Font>(materialAsset.Font);
            material->SetFont(font);
            material->SetBlendMode(static_cast<BlendMode>(materialAsset.BlendMode));
            material->SetCulling(materialAsset.IsCulling);
            material->SetDepthTest(materialAsset.IsDepthTest);
            material->SetDepthWrite(materialAsset.IsDepthWrite);

            if (!materialAsset.Image.IsEmpty()) {
                const auto sprite = factory.Get<Texture>(materialAsset.Image);
                material->AddSprite(sprite);
            }

            factory.GetCache()->Register(guid, material);
            return material;
        }
        return {};
    });

    // Mesh
    resources.RegisterLoader<Mesh>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<Mesh> {
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            if (auto cached = factory.GetCache()->Get<Mesh>(guid)) {
                return cached;
            }

            if (guid == SPRITE_GUID) {
                const auto meshAsset = std::shared_ptr<Mesh>(Mesh::GenerateSpritePtr());
                factory.GetCache()->Register(guid, meshAsset);
                return meshAsset;
            }

            const auto& meta = assetManager->GetMetaAsset(guid);
            if (factory.TryGenerateResource(meta.Type, guid)) {
                return factory.GetCache()->Get<Mesh>(guid);
            }

            const auto meshData = assetManager->LoadSourceByGuid<MeshData>(guid);
            std::shared_ptr<Mesh> mesh;
            if (meshData.HasSkeleton) {
                mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords,
                                              meshData.BoneWeights, meshData.BoneIndices);
                factory.GetCache()->RegisterValue(guid, meshData.BoneNames);
                factory.GetCache()->RegisterValue(guid, meshData.BoneOffsets);
                factory.GetCache()->RegisterValue(guid, meshData.BoneParents);
            } else {
                mesh = std::make_shared<Mesh>(meshData.Vertices, meshData.Indices, meshData.HasTexCoords);
            }
            factory.GetCache()->RegisterValue(guid, meshData.MeshBounds);
            factory.GetCache()->Register(guid, mesh);
            return mesh;
        }
        return {};
    });

    // Texture
    resources.RegisterLoader<Texture>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<Texture> {
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            return factory.GetCache()->GetOrLoad<Texture>(guid, [&]() {
                return std::make_shared<Texture>(assetManager->LoadSourceByGuid<Texture>(guid));
            });
        }
        return {};
    });

    // Font
    resources.RegisterLoader<Font>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<Font> {
        if (guid.IsEmpty())
            return {};
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            return factory.GetCache()->GetOrLoad<Font>(guid, [&]() {
                auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
                return std::make_shared<Font>(std::move(fontFile));
            });
        }
        return {};
    });

    // UiPage
    resources.RegisterLoader<UiPage>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<UiPage> {
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            if (auto cached = factory.GetCache()->Get<UiPage>(guid)) {
                return cached;
            }

            const auto uiPageAsset = assetManager->LoadAssetByGuid<UiPageAsset>(guid);

            const auto uiPage = std::make_shared<UiPage>();
            uiPage->Rml = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.RmlGuid);
            uiPage->Material = factory.Get<Material>(uiPageAsset.MaterialGuid);
            uiPage->FontPath = assetManager->GetPathFromGuid(uiPageAsset.FontGuid);
            uiPage->Css = assetManager->LoadSourceByGuid<std::string>(uiPageAsset.CssGuid);

            factory.GetCache()->Register(guid, uiPage);
            return uiPage;
        }
        return {};
    });
}
