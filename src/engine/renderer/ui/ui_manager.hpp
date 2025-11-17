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

                    // Initialize the render interface with window dimensions and asset path
                    render_interface.Initialize(_window, sceneManager->GetMaterial("48389756-b5e5-4e57-a44c-e85a030304c8"));

                    Rml::SetRenderInterface(&render_interface);
                    Rml::Initialise();

                    const auto fontPath = assetManager->GetPathFromGuid("682a2b0c-eeac-48f0-ab67-d2312a971cec");

                    Rml::LoadFontFace(fontPath);


                    // Create context
                    _rmlContext = Rml::CreateContext("main", Rml::Vector2i(window->GetWidth(), window->GetHeight()));

                    // Create a simple Hello World document
                    Rml::ElementDocument* document = _rmlContext->CreateDocument();
                    if (document) {
                        document->SetInnerRML(R"(
                    <div style="position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%);
                         font-family: arial; font-size: 48px; color: #FFFFFF; text-align: center;">
                        Hello World from RmlUi!
                    </div>
                )");
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
