#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "box_collider2d_component.hpp"
#include "box_collider2d_component_serialization.hpp"

class BoxCollider2dFactory final : public ComponentFactory<BoxCollider2DComponent, Box2dColliderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<BoxCollider2DComponent> &component, const Box2dColliderSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto world = scene->GetPhysicsWorld().lock()) {
                    if (const auto entity = _entity.lock()) {
                        b2PolygonShape dynamicBox;
                        // Half extents, then offset, then angle
                        dynamicBox.SetAsBox(
                            serialization.scale.x / 2,             // half width
                            serialization.scale.y / 2,             // half height
                            b2Vec2(serialization.translate.x, serialization.translate.y), // offset upward by half its height
                            0.0f                                   // rotation
                        );
                        b2FixtureDef fixtureDef;
                        fixtureDef.shape = &dynamicBox;
                        fixtureDef.density = 1.0f; // TODO move to serialization
                        fixtureDef.friction = 0.3f;  // TODO move to serialization
                        const auto rigidBody = entity->GetComponent<Rigidbody2dComponent>();
                        world->AddColliderComponent(rigidBody, component, fixtureDef);
                    }
                }
            }
        }
    }
};