#pragma once
#include "rigidbody2d_component.hpp"
#include "../entity.hpp"
#include "../../physics/physics_world.hpp"
#include "box2d/b2_body.h"


class BoxCollider2DComponent final : public Component
{

public:
    explicit BoxCollider2DComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {

    }

public:

    void Init(const glm::vec2 &size) const {
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(size.x / 2, size.y / 2);  // 2x2 box

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f; // TODO move to serialization
        fixtureDef.friction = 0.3f;  // TODO move to serialization

        const auto rigidBody = _entity.lock()->GetComponent<Rigidbody2dComponent>();

        if (const auto rig = rigidBody.lock()->GetBody().lock()) {
            rig->CreateFixture(&fixtureDef);
        }
    }

    void Update() const override
    {
    }
};
