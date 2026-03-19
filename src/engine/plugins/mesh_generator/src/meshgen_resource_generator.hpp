#pragma once
#include "resource_factory.hpp"
#include "mesh/mesh.hpp"
#include "bounds.hpp"
#include "meshgen_asset.hpp"
#include "meshgen_asset_yaml.hpp"
#include "mesh_generator.hpp"

inline void RegisterMeshGenResourceGenerator(ResourceFactory& resources) {
    resources.RegisterResourceGenerator("meshgen",
        [](ResourceFactory& factory, const Guid& guid) {
            if (const auto assetManager = factory.GetAssetManager().lock()) {
                const auto asset = assetManager->LoadAssetByGuid<MeshGenAsset>(guid);
                auto generated = MeshGenerator::Generate(asset, factory.GetAssetManager());

                if (generated.Vertices.empty() || generated.Indices.empty()) {
                    std::cerr << "[MeshGen] Empty mesh generated for: " << guid.ToString() << std::endl;
                    return;
                }

                auto format = generated.HasVertexColor ? VertexFormat::PositionColor : VertexFormat::PositionOnly;
                auto mesh = std::make_shared<Mesh>(generated.Vertices, generated.Indices, format);
                factory.GetCache()->RegisterValue(guid, generated.MeshBounds);
                factory.Register(guid, mesh);
            }
        }
    );
}
