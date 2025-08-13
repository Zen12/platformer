#include "engine.hpp"

#include <thread>

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

    _renderPipeline = std::make_shared<RenderPipeline>(_window);

    _sceneManager = std::make_shared<SceneManager>(_renderPipeline, _window,_assetManager, _inputSystem);

    _sceneManager->LoadScene(scene);

    _timer.Start();

    _renderPipeline->Init();

    if (const auto fpsText = _sceneManager->GetEntityById("text-fps").lock()) {
        _fpsText = fpsText->GetComponent<UiTextRenderer>();
    }
}

void Engine::WaitForTargetFrameRate() const {

    const float elapsed = _timer.GetResetDelta();

    const float sleepTime = _targetFrameTime - elapsed;

    if (sleepTime > 0.0) {
        std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
    }
}

Engine::~Engine() {
    AssetLoader::DeInit();
    _sceneManager->UnLoadAll();
    _window->Destroy();
}

void Engine::Tick() {

    const float deltaTime = _timer.GetResetDelta();
    _timer.Reset();

    if (const auto fpsText = _fpsText.lock()) {
        const float fps = 1.0f / deltaTime;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << fps;
        _fpsText.lock()->SetText(ss.str());
    }

    _inputSystem->Update();

    _sceneManager->Update(deltaTime);

    _renderPipeline->ClearFrame();
    _renderPipeline->RenderMeshes(deltaTime);
    _renderPipeline->RenderSprites(deltaTime);

#ifndef NDEBUG
    _renderPipeline->RenderDebugLines();
#endif
    _renderPipeline->RenderUI(deltaTime);

    _window->SwapBuffers();

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

