#pragma once
#include <memory>

#include "../../ui/button/button_component.hpp"
#include "../../game/game_state/game_state_component.hpp"
#include "../../scene/scene.hpp"

class BaseComponentFactory {
public:
    virtual ~BaseComponentFactory() = default;
};

template<typename TComponent, typename TSerialization>
class ComponentFactory : BaseComponentFactory {

static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>,
              "TSerialization must inherit from ComponentSerialization");

static_assert(std::is_base_of_v<Component, TComponent>,
          "TSerialization must inherit from ComponentSerialization");

protected:
    std::weak_ptr<Scene> _scene;
    std::weak_ptr<Entity> _entity;
    virtual void FillComponent(const std::weak_ptr<TComponent> &component, const TSerialization &serialization) = 0;

public:

    void SetScene(const std::weak_ptr<Scene> &scene) { _scene = scene; }
    void SetEntity(const std::weak_ptr<Entity> &entity) { _entity = entity; }

    void AddComponent(const std::weak_ptr<Entity>& entity, const TSerialization &serialization) {
        if (const auto e = entity.lock()) {
            const auto component = e->AddComponent<TComponent>();
            FillComponent(component, serialization);
        }
    }
};


















