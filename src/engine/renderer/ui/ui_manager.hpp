#pragma once
#include <cstdint>
#include <memory>

#include "opengl_render_interface.hpp"
#include "../../asset/asset_manager.hpp"

class UIManager final {
private:
    std::weak_ptr<AssetManager> _assetManager;
    std::unique_ptr<OpenGLRenderInterface> _render;

    int _viewportWidth = 0;
    int _viewportHeight = 0;

public:

    explicit UIManager(const std::weak_ptr<AssetManager> &assetManager) : _assetManager(assetManager)
    {}

    void Initialize(const uint16_t &width, const uint16_t & height) {
        _viewportWidth = width;
        _viewportHeight = height;

        _render = std::make_unique<OpenGLRenderInterface>();
    }
};
