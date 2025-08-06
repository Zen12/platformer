#pragma once
#include "../entity.hpp"
#include "../../physics/physics_world.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "glm/vec2.hpp"



class Rigidbody2dComponent final : public Component {
    std::shared_ptr<b2Body> _body;
    std::weak_ptr<PhysicsWorld> _world;

public:
    explicit Rigidbody2dComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {

    }

    [[nodiscard]] std::weak_ptr<b2Body> GetBody() const { return _body; }

    void SetWorld(const std::weak_ptr<PhysicsWorld> &world) {
        _world = world;
    }

    void AddForce(const glm::vec2 &forceValue) const {
        const b2Vec2 force(forceValue.x, forceValue.y);  // 100 N to the right
        const b2Vec2 point = _body->GetWorldCenter();  // Apply at center of mass

        _body->ApplyForce(force, point, true);
    }

    void Init(const bool& isDynamic) {
        if (const auto& world = _world.lock()) {

            auto def = b2BodyDef();
            if (isDynamic)
                def.type = b2_dynamicBody;
            else
                def.type = b2_staticBody;

            const auto position = _entity.lock()->GetComponent<Transform>().lock()->GetPosition();
            def.position.Set(position.x, position.y);

            b2Body *body = world->GetWorld().lock()->CreateBody(&def);

            // Wrap rawBody in shared_ptr with no-op deleter
            _body = std::shared_ptr<b2Body>(body, [](b2Body*) {
                // no delete, Box2D owns body lifetime
            });

            b2MassData massData;
            massData.mass = 5.0f;  // TODO move to serialization
            massData.center = body->GetLocalCenter();  // usually (0,0)
            massData.I = 1.0f;  // moment of inertia, set appropriately
            _body->SetMassData(&massData);

            body->SetLinearDamping(1.0f);
        }
    }

    void Update([[maybe_unused]] const float& deltaTime) override
    {
        const auto position = _body->GetPosition();
        _entity.lock()->GetComponent<Transform>().lock()->SetPosition(
            position.x, position.y);
    }
};
