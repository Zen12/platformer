#pragma once

#include "scene.hpp"
#include "scene_asset.hpp"
#include "scene_asset_yaml.hpp"
#include "../prefab/prefab_asset.hpp"
#include "../prefab/prefab_asset_yaml.hpp"
#include "../renderer/transform/rect_transform_root.hpp"
#include "../renderer/material/shader.hpp"
#include "../renderer/material/material.hpp"
#include "../renderer/render_pipeline.hpp"
#include "../asset/asset_manager.hpp"
#include "../physics/physics_world.hpp"
#include "../renderer/light_2d/light_2d_component.hpp"
#include "../physics/collider/box_collider2d_component.hpp"
#include "../physics/rigidbody/rigidbody2d_component.hpp"
#include "../debug/debug.hpp"
#include "../system/input_system.hpp"
#include "../system/guid_generator.hpp"

#include "../ui/text/text_renderer_component_factory.hpp"
#include "../ui/image/image_renderer_component_factory.hpp"
#include "../ui/button/button_component_factory.hpp"
#include "../ui/show_fps/show_fps_component_factory.hpp"
#include "../esc/esc.hpp"


class SceneManager {

    std::shared_ptr<Scene> _scene;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<Window> _window;
    std::weak_ptr<InputSystem> _inputSystem;

    std::unique_ptr<EscSystem> _escSystem{};

public:
    SceneManager(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem) {
        _assetManager = assetManager;
        _window = window;
        _inputSystem = inputSystem;
    }

    void LoadScene(const SceneAsset& serialization);
    void LoadEntities(const std::vector<EntitySerialization> &serialization);

    [[nodiscard]] std::weak_ptr<PhysicsWorld> GetPhysicsWorld() const {
        return _scene->GetPhysicsWorld();;
    }

    [[nodiscard]] std::weak_ptr<Entity> GetEntityById(const std::string& id) const;

    void CreateRequestedPrefabs() const;

    void Update(const float& deltaTime);

    void Render(const float &deltaTime) const;

    [[nodiscard]] bool IsRequestToLoadScene() const;

    void LoadRequestedScene();


private:
    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const;

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string& guid) const;


    [[nodiscard]] std::shared_ptr<Sprite> GetSprite(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string& guid) const;

    template <typename TSerialization, typename TFactory>
    void AddComponent(std::weak_ptr<Entity> e, TSerialization *serialization) const;

    template<typename TComponent, typename TFactory>
    bool TryToAddComponent(ComponentSerialization* component, std::weak_ptr<Entity> e) const;

    template<class SerializationT, class FactoryT, class ... Rest>
    bool TryToAddComponents(ComponentSerialization *comp, std::weak_ptr<Entity> e) const;
};


