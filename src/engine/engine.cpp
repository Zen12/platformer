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

    _sceneManager = std::make_shared<SceneManager>(_renderPipeline, _window,_assetManager);

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

    const auto character = _sceneManager->GetEntityById("main-character").lock()->GetComponent<Transform>().lock();
    const auto world =  _physicsWorld->GetWorld().lock();


    auto position = character->GetPosition();

    _inputSystem->Update();

    const auto speed = 2.0f;

    if (_inputSystem->IsKeyPressing(InputKey::W) || _inputSystem->IsKeyPress(InputKey::W)) {
        position.y += speed * deltaTime;
    }

    if (_inputSystem->IsKeyPressing(InputKey::S)|| _inputSystem->IsKeyPress(InputKey::S)) {
        position.y -= speed* deltaTime;
    }
    if (_inputSystem->IsKeyPressing(InputKey::A) || _inputSystem->IsKeyPress(InputKey::A)) {
        position.x -= speed * deltaTime;
    }
    if (_inputSystem->IsKeyPressing(InputKey::D) || _inputSystem->IsKeyPress(InputKey::D)) {
        position.x += speed * deltaTime;
    }

    character->SetPosition(position);

    _sceneManager->Update();

    _renderPipeline->ClearFrame();
    _renderPipeline->RenderMeshes();
    _renderPipeline->RenderSprites();

#ifndef NDEBUG
    _renderPipeline->RenderLines();
#endif
    _renderPipeline->RenderUI();

    _window->SwapBuffers();

#ifndef NDEBUG
    if (_inputSystem->IsKeyUp(InputKey::Escape)) {
        _window->Destroy();
    }
#endif
}

bool Engine::IsTickable() const {
    return _window->IsOpen();
}
