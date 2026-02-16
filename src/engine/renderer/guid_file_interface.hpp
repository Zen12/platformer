#pragma once
#include <RmlUi/Core/FileInterface.h>
#include <cstdio>
#include <string>
#include <memory>
#include <unordered_map>
#include "../asset/asset_manager.hpp"
#include "guid.hpp"

class GuidFileInterface final : public Rml::FileInterface {
private:
    std::weak_ptr<AssetManager> _assetManager;
    std::unordered_map<Rml::FileHandle, FILE*> _fileHandles;
    Rml::FileHandle _nextHandle = 1;

    std::string ResolveGuidToPath(const std::string& path) const {
        // Support both guid| and guid: formats (RmlUi converts | to : internally)
        std::string guidString;
        if (path.rfind("guid|", 0) == 0) {
            guidString = path.substr(5);
        } else if (path.rfind("guid:", 0) == 0) {
            guidString = path.substr(5);
        } else {
            return path;
        }

        const Guid guid = Guid::FromString(guidString);
        if (const auto assetManager = _assetManager.lock()) {
            return assetManager->GetPathFromGuid(guid);
        }

        std::cerr << "AssetManager not available for GUID resolution: " << guidString << std::endl;
        return "";
    }

public:
    explicit GuidFileInterface(const std::weak_ptr<AssetManager>& assetManager)
        : _assetManager(assetManager) {}

    Rml::FileHandle Open(const Rml::String& path) override {
        const std::string resolvedPath = ResolveGuidToPath(path);

        if (resolvedPath.empty()) {
            return 0;
        }

        FILE* fp = fopen(resolvedPath.c_str(), "rb");
        if (!fp) {
            return 0;
        }

        const Rml::FileHandle handle = _nextHandle++;
        _fileHandles[handle] = fp;
        return handle;
    }

    void Close(Rml::FileHandle file) override {
        const auto it = _fileHandles.find(file);
        if (it != _fileHandles.end()) {
            fclose(it->second);
            _fileHandles.erase(it);
        }
    }

    size_t Read(void* buffer, size_t size, Rml::FileHandle file) override {
        const auto it = _fileHandles.find(file);
        if (it != _fileHandles.end()) {
            return fread(buffer, 1, size, it->second);
        }
        return 0;
    }

    bool Seek(Rml::FileHandle file, long offset, int origin) override {
        const auto it = _fileHandles.find(file);
        if (it != _fileHandles.end()) {
            return fseek(it->second, offset, origin) == 0;
        }
        return false;
    }

    size_t Tell(Rml::FileHandle file) override {
        const auto it = _fileHandles.find(file);
        if (it != _fileHandles.end()) {
            return ftell(it->second);
        }
        return 0;
    }
};
