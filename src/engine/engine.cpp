#include "engine.hpp"
#include "register_all_plugins.hpp"
#include "scene_asset_yaml.hpp"
#include <RmlUi/Core.h>

#include <utility>

#include "fsm_asset.hpp"
#include "fsm_asset_yaml.hpp"
#include "fsm_factory.hpp"
#include "profiler.hpp"

#include "node/node_serialization_yaml.hpp"

#define DEBUG_ENGINE_PROFILE 0

#if DEBUG_ENGINE_PROFILE
#include <iostream>
#define ENGINE_LOG if(1) std::cout
#else
#define ENGINE_LOG if(0) std::cout
#endif

Engine::Engine(const std::filesystem::path &projectPath) : _projectPath(projectPath) {
    Profiler::Init();

    ENGINE_LOG << "Load project from: " << _projectPath << "\n";
    const std::filesystem::path projectFilePath = _projectPath.append("project.yaml");
    _projectAsset = AssetLoader<ProjectAsset>::LoadFromPath(projectFilePath);

    _assetManager = std::make_shared<AssetManager>(projectPath);
    _resourceFactory = std::make_shared<ResourceFactory>(_assetManager);
    _window = std::make_shared<Window>(_projectAsset.Resolution[0], _projectAsset.Resolution[1], _projectAsset.Name);
    _inputSystem = std::make_shared<InputSystem>(_window);
    _sceneManager = std::make_shared<SceneManager>(_window, _assetManager, _inputSystem, _resourceFactory);
    _uiManager = std::make_shared<UIManager>(_assetManager, _resourceFactory, _window);
    _renderController = std::make_shared<OpenGLRenderController>(_sceneManager, _resourceFactory);
    _videoRecorder = std::make_shared<VideoRecorder>();
    _audioManager = std::make_shared<AudioManager>(_assetManager);
    _sceneManager->SetAudioManager(_audioManager);

    // Auto-register all plugins
    PluginRegistries registries{_actionRegistry, _conditionRegistry, _componentRegistry, _systemRegistry, *_resourceFactory};
    RegisterAllPlugins(registries);

    // Register FsmAsset loader (core type, after plugins)
    _resourceFactory->RegisterLoader<FsmAsset>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<FsmAsset> {
        if (guid.IsEmpty())
            return {};
        if (const auto assetManager = factory.GetAssetManager().lock()) {
            return factory.GetCache()->GetOrLoad<FsmAsset>(guid, [&]() {
                return std::make_shared<FsmAsset>(assetManager->LoadAssetByGuid<FsmAsset>(guid));
            });
        }
        return {};
    });

    // Build engine context with all managers
    _engineContext.Register<UIManager>("UIManager", _uiManager);
    _engineContext.Register<SceneManager>("SceneManager", _sceneManager);
    _engineContext.Register<VideoRecorder>("VideoRecorder", _videoRecorder);
    _engineContext.Register<AudioManager>("AudioManager", _audioManager);

    // Set registry pointers for YAML deserialization
    YAML::convert<StateNodeSerialization>::s_actionRegistry = &_actionRegistry;
    YAML::convert<FsmAsset>::s_conditionRegistry = &_conditionRegistry;
    YAML::convert<EntitySerialization>::s_componentRegistry = &_componentRegistry;

    _window->WinInit();
    _audioManager->Initialize();
    _assetManager->Init();

    const auto mainFsm = _assetManager->LoadAssetByGuid<FsmAsset>(_projectAsset.MainFsm);
    _fsmController = FsmFactory::Create(mainFsm, _actionRegistry, _conditionRegistry, _engineContext);

    // Pass registries to scene manager for per-entity FSM creation
    _sceneManager->SetActionRegistry(&_actionRegistry);
    _sceneManager->SetConditionRegistry(&_conditionRegistry);
    _sceneManager->SetEngineContext(&_engineContext);
    _sceneManager->SetComponentRegistry(&_componentRegistry);
    _sceneManager->SetSystemRegistry(&_systemRegistry);

    _uiManager->Initialize();

    _targetFrameTime = 1.0f / _projectAsset.TargetFps;

#ifndef NDEBUG
        _audioManager->ToggleMute();
#endif
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
    ENGINE_LOG << "Destroying gameengine..." << std::endl;

    Profiler::Shutdown();

    // Shutdown audio first
    _audioManager->Shutdown();

    // Clean up RmlUi before destroying other resources
    // Reset the scene manager first (this releases the RmlUI context)
    _sceneManager.reset();

    // Destroy UI manager to release the context pointer before shutting down RmlUi
    _uiManager->Destroy();

    // Shutdown RmlUi (this will clean up all contexts)
    Rml::Shutdown();

    _window->Destroy();
}

void Engine::Tick() {
    PROFILE_SCOPE("Engine::Tick");

    _frameTimer.Reset();

    {
        PROFILE_SCOPE("Input");
        _inputSystem->Update();
    }

    {
        PROFILE_SCOPE("FSM");
        _fsmController->Update();
    }

    _renderController->Reset();

    // Forward mouse input to UI
    const auto mousePos = _inputSystem->GetMouseWindowPosition();
    _uiManager->ProcessMouseMove(static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));

    // Handle mouse button events for UI
    if (_inputSystem->IsMousePress(MouseButton::Left)) {
        _uiManager->ProcessMouseButtonDown(0); // 0 = left button in RmlUi
    }
    if (_inputSystem->IsMouseUp(MouseButton::Left)) {
        _uiManager->ProcessMouseButtonUp(0);
    }

    {
        PROFILE_SCOPE("Render");
        _renderController->Render(_sceneManager->GetRenderBuffer());
    }

    {
        PROFILE_SCOPE("UI");
        _uiManager->Update();
    }

    {
        PROFILE_SCOPE("Audio");
        _audioManager->Update();
    }

    // Capture frame for video recording if active
    if (_videoRecorder->IsRecording()) {
        _videoRecorder->CaptureFrame();
    }

    _sceneManager->ClearRenderBuffer();
    _window->SwapBuffers();

    if (_fsmController->IsSystemTriggered(SystemTriggers::Exit)) {
        _window->Destroy();
        return;
    }

    if (_fsmController->IsSystemTriggered(SystemTriggers::Reload)) {
        _isReloadRequested = true;
        _window->Destroy();
        return;
    }

    // maybe #if debug?
    if (_inputSystem->IsKeyUp(InputKey::Escape)) {
        _window->Destroy();
    }

    if (_inputSystem->IsKeyUp(InputKey::R)) {
        _isReloadRequested = true;
        _window->Destroy();
    }
    if (_inputSystem->IsKeyUp(InputKey::M)) {
        _audioManager->ToggleMute();
    }
}

bool Engine::IsTickable() const {
    return _window->IsOpen();
}

bool Engine::IsReloadRequested() const {
    return _isReloadRequested;
}
