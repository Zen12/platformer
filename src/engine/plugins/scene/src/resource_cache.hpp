#pragma once
#include "typed_cache.hpp"
#include <vector>
#include <memory>

#include "material/shader.hpp"
#include "material/material.hpp"
#include "mesh/mesh.hpp"
#include "bounds.hpp"
#include "asset_manager.hpp"
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
#include "guid.hpp"

class ResourceCache {
private:
    TypedCache<Shader> _shaders;
    TypedCache<Material> _materials;
    TypedCache<UiPage> _uiPages;
    TypedCache<Texture> _textures;
    TypedCache<Font> _fonts;
    TypedCache<Mesh> _meshes;
    TypedCache<AnimationData> _animations;
    ValueCache<std::vector<std::string>> _meshBoneNames;
    ValueCache<std::vector<glm::mat4>> _meshBoneOffsets;
    ValueCache<std::vector<int>> _meshBoneParents;
    ValueCache<Bounds> _meshBounds;

    std::weak_ptr<AssetManager> _assetManager;

public:
    explicit ResourceCache(const std::weak_ptr<AssetManager>& assetManager)
        : _assetManager(assetManager) {}

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const Guid& vertexGuid, const Guid& fragmentGuid);

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const Guid& guid);

    std::shared_ptr<Mesh> GetMesh(const Guid& guid);

    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const Guid& guid);

    [[nodiscard]] std::shared_ptr<Font> GetFont(const Guid& guid);

    [[nodiscard]] std::shared_ptr<UiPage> GetUiPage(const Guid& guid);

    [[nodiscard]] std::shared_ptr<AnimationData> GetAnimation(const Guid& guid);

    [[nodiscard]] std::vector<std::string> GetMeshBoneNames(const Guid& guid) const;

    [[nodiscard]] std::vector<glm::mat4> GetMeshBoneOffsets(const Guid& guid) const noexcept;

    [[nodiscard]] std::vector<int> GetMeshBoneParents(const Guid& guid) const noexcept;

    [[nodiscard]] Bounds GetMeshBounds(const Guid& guid) const noexcept;

    void RegisterMesh(const Guid& guid, std::shared_ptr<Mesh> mesh) noexcept {
        _meshes.Store(guid, std::move(mesh));
    }
};
