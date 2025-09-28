#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "rigidbody2d_component.hpp"
#include "rigidbody2d_component_serialization.hpp"

class Rigidbody2dFactory final : public ComponentFactory<Rigidbody2dComponent, Rigidbody2dSerialization> {
protected:
    void FillComponent(const std::weak_ptr<Rigidbody2dComponent> &component, const Rigidbody2dSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                if (const auto& world = scene->GetPhysicsWorld().lock()) {

                    auto def = b2BodyDef();
                    if (serialization.isDynamic)
                        def.type = b2_dynamicBody;
                    else
                        def.type = b2_staticBody;

                    const auto position = _entity.lock()->GetComponent<Transform>().lock()->GetPosition();
                    def.position.Set(position.x, position.y);

                    b2Body *body = world->GetWorld().lock()->CreateBody(&def);

                    b2MassData massData;
                    massData.mass = 5.0f;  // TODO move to serialization
                    massData.center = body->GetLocalCenter();  // usually (0,0)
                    massData.I = 1.0f;  // moment of inertia, set appropriately
                    body->SetMassData(&massData);

                    body->SetLinearDamping(1.0f);

                    world->AddRigidBodyComponent(component, body);
                }
            }
        }
    }
};