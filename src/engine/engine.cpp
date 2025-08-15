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

    _sceneManager = std::make_shared<SceneManager>(_window,_assetManager, _inputSystem);
    _sceneManager->LoadScene(scene);

    _frameTimer.Start();

    if (const auto fpsText = _sceneManager->GetEntityById("text-fps").lock()) {
        _fpsText = fpsText->GetComponent<UiTextRenderer>();
    }
}

void Engine::WaitForTargetFrameRate() const {

    const float elapsed = _frameTimer.GetResetDelta();

    const float sleepTime = _targetFrameTime - elapsed;

    if (sleepTime > 0.0) {
        std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
    }
}

Engine::~Engine() {
    std::cout << "Destroying gameengine..." << std::endl;
    _window->Destroy();
}

void Engine::Tick() {

    const float deltaTime = _frameTimer.GetResetDelta();
    _frameTimer.Reset();

    if (const auto fpsText = _fpsText.lock()) {
        const float fps = 1.0f / deltaTime;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << fps;
        _fpsText.lock()->SetText(ss.str());
    }

    _inputSystem->Update();

    _sceneManager->Update(deltaTime);



    _window->SwapBuffers();

/*
    if (const auto camera3d = _renderPipeline->Get3dCamera().lock()) {
        const auto mousePosition = _inputSystem->GetMouseScreenSpace();
        const auto worldPosition = camera3d->ScreenToWorldPoint(mousePosition);

        DebugLines::AddLine(worldPosition, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    */


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

