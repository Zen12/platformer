#pragma once
#include <memory>

#include "box_collider_component.hpp"
#include "physics_world_component.hpp"
#include "../esc_core.hpp"
#include "../transform/transform_component.hpp"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

class BoxColliderSystem final : public ISystemView<BoxColliderComponent, TransformComponentV2>{
private:
    using TypeWorld = decltype(std::declval<entt::registry&>().view<PhysicsWorldComponent>());
    TypeWorld _physicsWorldView;

public:
    explicit BoxColliderSystem(const TypeView &view, const TypeWorld &world)
        : ISystemView(view), _physicsWorldView(world)
    {}

    void OnTick() override {
        for (auto &&[_, world] : _physicsWorldView.each()) {
            for (const auto &[_, component, transform]: View.each()) {
                const auto position = transform.GetPosition();

                if (component.Id < 1) {
                    // Create new shape and store it
                    auto dynamicBox = std::make_unique<b2PolygonShape>();
                    // Half extents, then offset, then angle
                    dynamicBox->SetAsBox(
                        component.Extents.x / 2,             // half width
                        component.Extents.y / 2,             // half height
                        b2Vec2(position.x, position.y), // offset upward by half its height
                        0.0f                                   // rotation
                    );

                    auto fixtureDef = std::make_unique<b2FixtureDef>();
                    fixtureDef->shape =  dynamicBox.get();
                    fixtureDef->density = 1.0f; // TODO move to serialization
                    fixtureDef->friction = 0.3f;  // TODO move to serialization

                     component.Id = world.Add(std::move(fixtureDef), std::move(dynamicBox));
                } else {
                    world.UpdateShape(component.Id, position, component.Extents);
                }
            }
        }
    }

};


