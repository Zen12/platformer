#pragma once
#include <cstdint>
#include <memory>
#include <functional>

#include "controller/ui_opengl_render_controller.hpp"
#include "../asset/asset_manager.hpp"
#include "../scene/scene_manager.hpp"
#include "../system/guid.hpp"
#include "guid_file_interface.hpp"
#include "RmlUi/Core/Core.h"
#include "RmlUi/Core/ElementDocument.h"
#include "RmlUi/Core/EventListener.h"
#include "RmlUi/Core/Elements/ElementProgress.h"

// Custom event listener for button clicks
class ButtonClickListener : public Rml::EventListener {
private:
    std::function<void()>* _handler;

public:
    explicit ButtonClickListener(std::function<void()>* handler) : _handler(handler) {}

    void ProcessEvent([[maybe_unused]] Rml::Event& event) override {
        if (_handler && *_handler) {
            (*_handler)();
        }
    }
};

struct ButtonListenerData {
    std::function<void()> handler;
    std::unique_ptr<ButtonClickListener> listener;
    Rml::Element* element = nullptr;
};

class UIManager final {
private:
    const std::weak_ptr<SceneManager> _sceneManager{};
    const std::weak_ptr<Window> _window{};
    const std::weak_ptr<AssetManager> _assetManager{};

    std::unique_ptr<UiOpenGLRenderController> _render{};
    std::unique_ptr<GuidFileInterface> _fileInterface{};
    std::unique_ptr<Rml::Context> _rmlContext{};

    bool _isPageLoaded{};
    std::unordered_map<int, ButtonListenerData> _buttonListeners;
    int _nextHandlerId = 0;


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
        _fileInterface = std::make_unique<GuidFileInterface>(_assetManager);

        Rml::SetFileInterface(_fileInterface.get());
        Rml::SetRenderInterface(_render.get());
        Rml::Initialise();
    }

    void LoadPage(const Guid& guid) {
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
                _render->Initialize(_window, uiPage->Material, _assetManager);
                Rml::LoadFontFace(uiPage->FontPath);
                const auto css = Rml::Factory::InstanceStyleSheetString(uiPage->Css);
                // Create context
                _rmlContext = std::unique_ptr<Rml::Context>(
                    Rml::CreateContext("main", Rml::Vector2i(window->GetWidth(), window->GetHeight())));

                // Load document from memory to properly process templates
                if (Rml::ElementDocument* document = _rmlContext->LoadDocumentFromMemory(uiPage->Rml)) {
                    document->SetStyleSheetContainer(css);
                    document->Show();
                }
            }
        }
    }

    void UnLoadPage([[maybe_unused]] const Guid &guid) {
        Rml::RemoveContext("main");
        _rmlContext.release();
        _isPageLoaded = false;
    }

    void ProcessMouseMove(const int &x, const int &y) {
        if (_rmlContext) {
            _rmlContext->ProcessMouseMove(x, y, 0);
        }
    }

    void ProcessMouseButtonDown(const int &button) {
        if (_rmlContext) {
            _rmlContext->ProcessMouseButtonDown(button, 0);
        }
    }

    void ProcessMouseButtonUp(const int &button) {
        if (_rmlContext) {
            _rmlContext->ProcessMouseButtonUp(button, 0);
        }
    }

    int SetButtonClickHandler(const std::string& buttonIdentifier, std::function<void()> handler) {
        int handlerId = _nextHandlerId++;

        // If page is already loaded, attach listener immediately
        if (_rmlContext) {
            if (Rml::ElementDocument* document = _rmlContext->GetDocument(0)) {
                // Try to find element by ID first, then by tag name
                Rml::Element* buttonElement = document->GetElementById(buttonIdentifier);
                if (!buttonElement) {
                    Rml::ElementList elements;
                    document->GetElementsByTagName(elements, buttonIdentifier);
                    if (!elements.empty()) {
                        buttonElement = elements[0];
                    }
                }

                if (buttonElement) {
                    // Create listener data and insert into map first to get stable address
                    auto& listenerData = _buttonListeners[handlerId];
                    listenerData.handler = std::move(handler);
                    listenerData.element = buttonElement;
                    listenerData.listener = std::make_unique<ButtonClickListener>(&listenerData.handler);

                    // Attach listener to element
                    buttonElement->AddEventListener(Rml::EventId::Click, listenerData.listener.get());
                }
            }
        }

        return handlerId;
    }

    void RemoveButtonClickHandler(int handlerId) {
        auto it = _buttonListeners.find(handlerId);
        if (it != _buttonListeners.end()) {
            auto& listenerData = it->second;
            if (listenerData.element && listenerData.listener) {
                listenerData.element->RemoveEventListener(Rml::EventId::Click, listenerData.listener.get());
            }
            _buttonListeners.erase(it);
        }
    }

    void UpdateElementText(const std::string& elementId, const std::string& text) {
        if (_rmlContext) {
            if (Rml::ElementDocument* document = _rmlContext->GetDocument(0)) {
                if (Rml::Element* element = document->GetElementById(elementId)) {
                    element->SetInnerRML(text);
                }
            }
        }
    }

    void UpdateProgressValue(const std::string& elementId, float value, float max = 1.0f) {
        if (_rmlContext) {
            if (Rml::ElementDocument* document = _rmlContext->GetDocument(0)) {
                if (Rml::Element* element = document->GetElementById(elementId)) {
                    if (auto* progress = dynamic_cast<Rml::ElementProgress*>(element)) {
                        progress->SetMax(max);
                        progress->SetValue(value);
                    }
                }
            }
        }
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
        // Remove all event listeners before cleanup
        for (auto& [identifier, listenerData] : _buttonListeners) {
            if (listenerData.element && listenerData.listener) {
                listenerData.element->RemoveEventListener(Rml::EventId::Click, listenerData.listener.get());
            }
        }
        _buttonListeners.clear();

        // Release the unique_ptr without calling the deleter
        // Rml::Shutdown() will handle the actual context cleanup
        if (_rmlContext != nullptr) {
            _rmlContext.release();
        }
    }

};
