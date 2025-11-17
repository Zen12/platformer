#include "engine.hpp"
#include "scene/scene_asset_yaml.hpp"
#include <RmlUi/Core.h>

#define DEBUG_ENGINE_PROFILE 0

Engine::Engine(const std::filesystem::path &projectPath) : _projectPath(projectPath) {

    std::cout << "Load project from: " << _projectPath << "\n";
    AssetLoader<Texture>::Init();
    const std::filesystem::path projectFilePath = _projectPath.append("project.yaml");
    _projectAsset = AssetLoader<ProjectAsset>::LoadFromPath(projectFilePath);

    _assetManager = std::make_shared<AssetManager>(projectPath);
    _window = std::make_shared<Window>(_projectAsset.Resolution[0], _projectAsset.Resolution[1], _projectAsset.Name);
    _inputSystem = std::make_shared<InputSystem>(_window);
    _sceneManager = std::make_shared<SceneManager>(_window, _assetManager, _inputSystem);
    _uiManager = std::make_shared<UIManager>(_assetManager, _sceneManager, _window);

    _window->WinInit();
    _assetManager->Init();

    _targetFrameTime = 1.0f / _projectAsset.TargetFps;
}

void Engine::LoadFirstScene() {
    const auto scene = _assetManager->LoadAssetByGuid<SceneAsset>(_projectAsset.Scenes[0]);

    _sceneManager->LoadScene(scene);

    _uiManager->Initialize();

    _frameTimer.Start();
}

void Engine::WaitForTargetFrameRate() const {
    const float elapsed = _frameTimer.GetResetDelta();
    const float sleepTime = _targetFrameTime - elapsed;

    if (sleepTime > std::numeric_limits<float>::epsilon()) {
        if (sleepTime > 0.002f) {
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime - 0.001f));
        }

        while (_frameTimer.GetResetDelta() < _targetFrameTime) {
            // spin
        }
    }
}

Engine::~Engine() {
    std::cout << "Destroying gameengine..." << std::endl;

    // Clean up RmlUi before destroying other resources
    // Reset the scene manager first (this releases the RmlUI context)
    _sceneManager.reset();

    // Destroy UI manager before shutting down RmlUi
    _uiManager->Destroy();

    // Now it's safe to shutdown RmlUi
    Rml::Shutdown();

    _window->Destroy();
}

void Engine::Tick() {
#if DEBUG_ENGINE_PROFILE
    PROFILE_SCOPE("Engine::Tick");
#endif

    _frameTimer.Reset();

    if (_sceneManager->IsRequestToLoadScene()) {
        _sceneManager->LoadRequestedScene();
    }

    _inputSystem->Update();
    _sceneManager->Update();
    _uiManager->Update();
    _window->SwapBuffers();

    // maybe #if debug?
    if (_inputSystem->IsKeyUp(InputKey::Escape)) {
        _window->Destroy();
    }

    if (_inputSystem->IsKeyUp(InputKey::R)) {
        _isReloadRequested = true;
        _window->Destroy();
    }
}

bool Engine::IsTickable() const {
    return _window->IsOpen();
}

bool Engine::IsReloadRequested() const {
    return _isReloadRequested;
}

