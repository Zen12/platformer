#pragma once
#include <cstdint>
#include <memory>

#include "opengl_render_interface.hpp"
#include "../../asset/asset_manager.hpp"
#include "../../scene/scene_manager.hpp"
#include "RmlUi/Core/Core.h"
#include "RmlUi/Core/ElementDocument.h"

class UIManager final {
private:
    const std::weak_ptr<SceneManager> _sceneManager{};
    const std::weak_ptr<Window> _window{};
    const std::weak_ptr<AssetManager> _assetManager{};

    std::unique_ptr<OpenGLRenderInterface> _render{};
    Rml::Context* _rmlContext{};


public:

    UIManager(
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<SceneManager> &sceneManager,
        const std::weak_ptr<Window> &window
        ) : _sceneManager(sceneManager), _window(window) , _assetManager(assetManager),
            _render(std::make_unique<OpenGLRenderInterface>())
    {}

    void Initialize() {
        if (const auto &sceneManager = _sceneManager.lock()) {
            if (const auto &assetManager = _assetManager.lock()) {
                if (const auto window = _window.lock()) {

                    _render = std::make_unique<OpenGLRenderInterface>();

                    static OpenGLRenderInterface render_interface;

                    const auto uiPage = sceneManager->GetUiPage("df04e927-64d5-406b-8170-ddd96f1864a5");

                    // Initialize the render interface with window dimensions and asset path
                    render_interface.Initialize(_window, uiPage->Material);

                    Rml::SetRenderInterface(&render_interface);
                    Rml::Initialise();

                    std::cout << uiPage->FontPath << std::endl;

                    Rml::LoadFontFace(uiPage->FontPath);
                    const auto css = Rml::Factory::InstanceStyleSheetString(uiPage->Css);


                    // Create context
                    _rmlContext = Rml::CreateContext("main", Rml::Vector2i(window->GetWidth(), window->GetHeight()));

                    // Create a simple Hello World document
                    if (Rml::ElementDocument* document = _rmlContext->CreateDocument()) {
                        document->SetInnerRML(uiPage->Rml);
                        document->SetStyleSheetContainer(std::move(css));
                        document->Show();
                    }
                }
            }
        }
    }

    void Update() {
        // Render RmlUi with proper 2D OpenGL state
        if (_rmlContext) {
            _rmlContext->Update();
            _rmlContext->Render();
        }
    }

    void Destroy() {
        if (_rmlContext != nullptr) {
            const auto name = _rmlContext->GetName();
            _rmlContext = nullptr;
            Rml::RemoveContext(name);
        }
    }

};
