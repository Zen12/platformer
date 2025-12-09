#pragma once
#include <cstdint>
#include <memory>

#include "controller/ui_opengl_render_controller.hpp"
#include "../asset/asset_manager.hpp"
#include "../scene/scene_manager.hpp"
#include "RmlUi/Core/Core.h"
#include "RmlUi/Core/ElementDocument.h"

class UIManager final {
private:
    const std::weak_ptr<SceneManager> _sceneManager{};
    const std::weak_ptr<Window> _window{};
    const std::weak_ptr<AssetManager> _assetManager{};

    std::unique_ptr<UiOpenGLRenderController> _render{};
    std::unique_ptr<Rml::Context> _rmlContext{};

    bool _isPageLoaded{};


public:

    UIManager(
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<SceneManager> &sceneManager,
        const std::weak_ptr<Window> &window
        ) : _sceneManager(sceneManager), _window(window) , _assetManager(assetManager),
            _render(std::make_unique<UiOpenGLRenderController>())
    {}

    void Initialize() {
        _render = std::make_unique<UiOpenGLRenderController>();

        Rml::SetRenderInterface(_render.get());
        Rml::Initialise();
    }

    void LoadPage(const std::string& guid) {
        //df04e927-64d5-406b-8170-ddd96f1864a5
        if (_isPageLoaded)
            return;

        // no support for multiple pages
        if (const auto &sceneManager = _sceneManager.lock()) {
            if (const auto window = _window.lock()) {
                const auto uiPage = sceneManager->GetUiPage(guid);
                if (!uiPage) {
                    // No scene loaded yet, skip UI page loading
                    return;
                }
                _isPageLoaded = true;

                // Initialize the render interface with window dimensions and asset path
                _render->Initialize(_window, uiPage->Material);
                Rml::LoadFontFace(uiPage->FontPath);
                const auto css = Rml::Factory::InstanceStyleSheetString(uiPage->Css);
                // Create context
                _rmlContext = std::unique_ptr<Rml::Context>(
                    Rml::CreateContext("main", Rml::Vector2i(window->GetWidth(), window->GetHeight())));

                // Create a simple Hello World document
                if (Rml::ElementDocument* document = _rmlContext->CreateDocument()) {
                    document->SetInnerRML(uiPage->Rml);
                    document->SetStyleSheetContainer(css);
                    document->Show();
                }
            }
        }
    }

    void UnLoadPage(const std::string &guid) {
        Rml::RemoveContext(guid);
        _isPageLoaded = false;
    }

    void Update() {
        // Render RmlUi with proper 2D OpenGL state
        if (_rmlContext) {
            if (const auto window = _window.lock()) {
                _rmlContext->SetDimensions(Rml::Vector2i(window->GetWidth(), window->GetHeight()));
            }
            _rmlContext->Update();
            _rmlContext->Render();
        }
    }

    void Destroy() {
        // Release the unique_ptr without calling the deleter
        // Rml::Shutdown() will handle the actual context cleanup
        if (_rmlContext != nullptr) {
            _rmlContext.release();
        }
    }

};
