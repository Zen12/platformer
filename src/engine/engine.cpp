#include "engine.hpp"

#define DEBUG_ENGINE_PROFILE 0

Engine::Engine(const std::filesystem::path &projectPath) : _projectPath(projectPath) {

    std::cout << "Load project from: " << _projectPath << "\n";
    AssetLoader::Init();
    const std::filesystem::path projectFilePath = _projectPath.append("project.yaml");
    _projectAsset = AssetLoader::LoadFromPath<ProjectAsset>(projectFilePath);

    _assetManager = std::make_shared<AssetManager>(projectPath);
    _assetManager->Init();

    _window = std::make_shared<Window>(_projectAsset.Resolution[0], _projectAsset.Resolution[1], _projectAsset.Name);
    _window->WinInit();

    _inputSystem = std::make_shared<InputSystem>(_window);
    _targetFrameTime = 1.0f / _projectAsset.TargetFps;
}

void Engine::LoadFirstScene() {
    const auto scene = _assetManager->LoadAssetByGuid<SceneAsset>(_projectAsset.Scenes[0]);

    _sceneManager = std::make_shared<SceneManager>(_window,_assetManager, _inputSystem);
    _sceneManager->LoadScene(scene);

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
    _window->Destroy();
}

void Engine::Tick() {
#ifndef NDEBUG
#if DEBUG_ENGINE_PROFILE
    PROFILE_SCOPE("Engine::Tick");
#endif
#endif

    if (_sceneManager->IsRequestToLoadScene()) {
        _sceneManager->LoadRequestedScene();
    }

    const float deltaTime = _frameTimer.GetResetDelta();
    _frameTimer.Reset();
    _inputSystem->Update();
    _sceneManager->Update(deltaTime);
    _sceneManager->Render(deltaTime);
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

