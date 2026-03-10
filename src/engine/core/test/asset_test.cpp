#include <gtest/gtest.h>
#include "meta/meta_asset.hpp"
#include "meta/meta_asset_yaml.hpp"
#include "loaders/asset_loader.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>

TEST(MetaAsset, DefaultConstruction) {
    MetaAsset meta;
    EXPECT_TRUE(meta.Guid.IsEmpty());
    EXPECT_TRUE(meta.Extension.empty());
    EXPECT_TRUE(meta.Type.empty());
    EXPECT_TRUE(meta.Path.empty());
}

TEST(MetaAsset, YamlDeserialization) {
    YAML::Node node;
    node["guid"] = "550e8400-e29b-41d4-a716-446655440000";
    node["extension"] = ".png";
    node["type"] = "image";

    MetaAsset meta = node.as<MetaAsset>();

    EXPECT_FALSE(meta.Guid.IsEmpty());
    EXPECT_EQ(meta.Extension, ".png");
    EXPECT_EQ(meta.Type, "image");
}

TEST(AssetLoader, LoadExistingFile) {
    namespace fs = std::filesystem;

    fs::path tempDir = fs::temp_directory_path();
    fs::path tempFile = tempDir / "test_asset_loader.txt";

    std::ofstream out(tempFile);
    out << "test content";
    out.close();

    std::string content = AssetLoader<std::string>::LoadFromPath(tempFile.string());
    EXPECT_EQ(content, "test content");

    fs::remove(tempFile);
}

TEST(AssetLoader, LoadNonExistentFileReturnsEmpty) {
    std::string content = AssetLoader<std::string>::LoadFromPath("/nonexistent/path/file.txt");
    EXPECT_TRUE(content.empty());
}

TEST(AssetLoader, HasAssetLoaderTypeTrait) {
    EXPECT_TRUE(has_asset_loader_v<std::string>);
}
