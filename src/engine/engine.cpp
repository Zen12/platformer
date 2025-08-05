#include "engine.hpp"

Engine::Engine(const std::string &projectPath) : _projectPath(projectPath) {

    AssetLoader::Init();
    _projectAsset = AssetLoader::LoadFromPath<ProjectAsset>(projectPath + "project.yaml");

    _assetManager = std::make_shared<AssetManager>(projectPath);
    _assetManager->Init();

    _window = std::make_shared<Window>(_projectAsset.Resolution[0], _projectAsset.Resolution[1], _projectAsset.Name);
    _window->WinInit();

    _inputSystem = std::make_shared<InputSystem>(_window);
}

void Engine::LoadFirstScene() {
    const auto scene = _assetManager->LoadAssetByGuid<SceneAsset>(_projectAsset.Scenes[0]);

    _renderPipeline = std::make_shared<RenderPipeline>(_window);

    _sceneManager = std::make_shared<SceneManager>(_renderPipeline, _window,_assetManager, _inputSystem);

    _sceneManager->LoadScene(scene);

    _physicsWorld = _sceneManager->GetPhysicsWorld();

    _timer.Start();

    _renderPipeline->Init();

    _fpsText = _sceneManager->GetEntityById("text-fps").lock()->GetComponent<UiTextRenderer>().lock();
}

Engine::~Engine() {
    AssetLoader::DeInit();
    _sceneManager->UnLoadAll();
    _window->Destroy();
}

void Engine::Tick() {

    const float deltaTime = _timer.GetResetDelta();
    _timer.Reset();

    const float fps = 1.0f / deltaTime;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << fps;
    _fpsText.lock()->SetText(ss.str());

    const auto world =  _physicsWorld->GetWorld().lock();

    _inputSystem->Update();

    _sceneManager->Update(deltaTime);

    _renderPipeline->ClearFrame();
    _renderPipeline->RenderMeshes(deltaTime);
    _renderPipeline->RenderSprites(deltaTime);

#ifndef NDEBUG
    _renderPipeline->RenderLines(deltaTime);
#endif
    _renderPipeline->RenderUI(deltaTime);

    _window->SwapBuffers();

#ifndef NDEBUG
    if (_inputSystem->IsKeyUp(InputKey::Escape)) {
        _window->Destroy();
    }

    if (_inputSystem->IsKeyUp(InputKey::R)) {
        _isReloadRequested = true;
        _window->Destroy();
    }
#endif
}

bool Engine::IsTickable() const {
    return _window->IsOpen();
}

bool Engine::IsReloadRequested() const {
    return _isReloadRequested;
}

