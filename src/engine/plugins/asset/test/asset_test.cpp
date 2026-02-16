#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "meta/meta_asset.hpp"
#include "meta/meta_asset_yaml.hpp"
#include "loaders/asset_loader.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>

TEST_CASE("MetaAsset default construction") {
    MetaAsset meta;
    CHECK(meta.Guid.IsEmpty());
    CHECK(meta.Extension.empty());
    CHECK(meta.Type.empty());
    CHECK(meta.Path.empty());
}

TEST_CASE("MetaAsset YAML deserialization") {
    YAML::Node node;
    node["guid"] = "550e8400-e29b-41d4-a716-446655440000";
    node["extension"] = ".png";
    node["type"] = "image";

    MetaAsset meta = node.as<MetaAsset>();

    CHECK_FALSE(meta.Guid.IsEmpty());
    CHECK(meta.Extension == ".png");
    CHECK(meta.Type == "image");
}

TEST_CASE("AssetLoader string specialization") {
    namespace fs = std::filesystem;

    fs::path tempDir = fs::temp_directory_path();
    fs::path tempFile = tempDir / "test_asset_loader.txt";

    SUBCASE("Load existing file") {
        std::ofstream out(tempFile);
        out << "test content";
        out.close();

        std::string content = AssetLoader<std::string>::LoadFromPath(tempFile.string());
        CHECK(content == "test content");

        fs::remove(tempFile);
    }

    SUBCASE("Load non-existent file returns empty") {
        std::string content = AssetLoader<std::string>::LoadFromPath("/nonexistent/path/file.txt");
        CHECK(content.empty());
    }
}

TEST_CASE("has_asset_loader_v type trait") {
    CHECK(has_asset_loader_v<std::string>);
}
