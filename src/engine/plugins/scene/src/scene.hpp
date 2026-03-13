#pragma once
#include <optional>
#include <unordered_map>
#include <vector>

#include "resource_factory.hpp"
#include "input_system.hpp"
#include "mesh/mesh.hpp"
#include "entt/entity/registry.hpp"
#include "skybox/skybox_renderer.hpp"
#include "guid.hpp"

class AudioManager;
class FsmActionRegistry;
class FsmConditionRegistry;
class EngineContext;

class Scene {
private:
    std::shared_ptr<ResourceFactory> _resourceFactory;

    std::weak_ptr<Window> _window;
    std::weak_ptr<InputSystem> _inputSystem;

    const FsmActionRegistry* _actionRegistry = nullptr;
    const FsmConditionRegistry* _conditionRegistry = nullptr;
    const EngineContext* _engineContext = nullptr;

    std::shared_ptr<entt::registry> _entityRegistry{};
    std::unique_ptr<SkyboxRenderer> _skyboxRenderer{};

    Guid _requestToLoadScene{};

public:

    Scene(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<InputSystem> &inputSystem,
        const std::shared_ptr<ResourceFactory> &resourceFactory)
        :  _resourceFactory(resourceFactory),
           _window(window), _inputSystem(inputSystem)

    {

        _entityRegistry = std::make_shared<entt::registry>();
    }


    [[nodiscard]] std::weak_ptr<InputSystem> GetInputSystem() const noexcept;

    [[nodiscard]] std::weak_ptr<Window> GetWindow() const noexcept;

    void SetActionRegistry(const FsmActionRegistry* reg) noexcept { _actionRegistry = reg; }
    void SetConditionRegistry(const FsmConditionRegistry* reg) noexcept { _conditionRegistry = reg; }
    void SetEngineContext(const EngineContext* ctx) noexcept { _engineContext = ctx; }

    [[nodiscard]] const FsmActionRegistry* GetActionRegistry() const noexcept { return _actionRegistry; }
    [[nodiscard]] const FsmConditionRegistry* GetConditionRegistry() const noexcept { return _conditionRegistry; }
    [[nodiscard]] const EngineContext* GetEngineContext() const noexcept { return _engineContext; }

    [[nodiscard]] std::shared_ptr<entt::registry> GetEntityRegistry() const noexcept {
        return _entityRegistry;
    }

    [[nodiscard]] SkyboxRenderer* GetSkyboxRenderer() const noexcept {
        return _skyboxRenderer.get();
    }

    void InitializeSkybox(const Guid& materialGuid) {
        if (materialGuid.IsEmpty()) {
            _skyboxRenderer.reset();
            return;
        }
        _skyboxRenderer = std::make_unique<SkyboxRenderer>();
        _skyboxRenderer->Initialize(materialGuid);
    }

    [[nodiscard]] std::shared_ptr<ResourceFactory> GetResourceFactory() const noexcept {
        return _resourceFactory;
    }

    void RequestToLoadScene(const Guid &sceneGuid) noexcept {
        _requestToLoadScene = sceneGuid;
    }

    [[nodiscard]] Guid GetLoadSceneRequestGuid() const noexcept {
        return _requestToLoadScene;
    }

    void RegisterMesh(const Guid &guid, std::shared_ptr<Mesh> mesh) noexcept {
        _resourceFactory->Register<Mesh>(guid, std::move(mesh));
    }
};
