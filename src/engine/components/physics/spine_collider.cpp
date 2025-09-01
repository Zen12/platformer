#include "spine_collider.hpp"

#define DEBUG_ENGINE_SPINE_COLLIDER_PROFILE 1

void SpineColliderComponent::CreateColliderFixture(spine::Bone *bone, const std::weak_ptr<BoxCollider2DComponent> &collider) const {
    if (const auto entity = _entity.lock()) {
        if (const auto tr = entity->GetComponent<Transform>().lock()) {
            if (const auto world = _physicsWorld.lock()) {
                if (const auto render = _renderer.lock()) {
                    auto [start, end] = render->GetBoneEndpoints(bone);
                    const auto pos = tr->GetPosition();
                    start -= pos; // need local pos
                    end -= pos;// need local pos

                    // Convert glm::vec3 → Box2D vec2
                    const b2Vec2 startB2(start.x, start.y);
                    const b2Vec2 endB2(end.x, end.y);

                    // Midpoint (center of collider)
                    const b2Vec2 center = 0.5f * (startB2 + endB2);

                    // Direction vector
                    const b2Vec2 dir = endB2 - startB2;
                    const float length = dir.Length();
                    const float angle = atan2f(dir.y, dir.x);

                    // Half-extents
                    const float halfLength = length * 0.5f;
                    const float halfThickness = 0.10f; // choose thickness you want

                    // Box2D polygon shape (box aligned with bone)
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox(
                        halfLength,           // half-width = half the bone length
                        halfThickness,        // half-height = thickness
                        center,               // center of the box
                        angle                 // rotation (in radians)
                    );


                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;

                    const auto rigidBody = entity->GetComponent<Rigidbody2dComponent>();
                    world->AddColliderComponent(rigidBody, collider, fixtureDef);
                }
            }
        }
    }
}

std::weak_ptr<BoxCollider2DComponent> SpineColliderComponent::CreateCollider(spine::Bone *bone) const {
#ifndef NDEBUG
#if DEBUG_ENGINE_SPINE_COLLIDER_PROFILE
    PROFILE_SCOPE("    SpineColliderComponent::CreateCollider");
#endif
#endif
    if (const auto entity = _entity.lock()) {
        const auto collider = entity->AddComponent<BoxCollider2DComponent>();
        CreateColliderFixture(bone, collider);
        return collider;
    }
    return {};
}

void SpineColliderComponent::SetPhysicsWorld(const std::weak_ptr<PhysicsWorld> &physicsWorld) noexcept {
    _physicsWorld = physicsWorld;
}

void SpineColliderComponent::SetRenderer(const std::weak_ptr<SpineRenderer> &renderer) noexcept {
    _renderer = renderer;
}

void SpineColliderComponent::SetRigidBody2d(const std::weak_ptr<Rigidbody2dComponent> &rigidbody2d) noexcept {
    _rigidbody2d = rigidbody2d;
}

void SpineColliderComponent::Update([[maybe_unused]] const float &deltaTime) {
#ifndef NDEBUG
#if DEBUG_ENGINE_SPINE_COLLIDER_PROFILE
    PROFILE_SCOPE("    SpineColliderComponent::Update");
#endif
#endif
    if (const auto world = _physicsWorld.lock()) {
        if (const auto entity = _entity.lock()) {
            if (const auto render = _renderer.lock()) {
                const auto bones = render->GetBones();

                for (size_t i = 0; i < bones.size(); i++) {
                    if (const auto bone = bones[i]; _bonesColliders.find(bone) == _bonesColliders.end()) {
                        _bonesColliders[bone] =  CreateCollider(bone);;
                    } else {
#ifndef NDEBUG
#if DEBUG_ENGINE_SPINE_COLLIDER_PROFILE
                        PROFILE_SCOPE("    SpineColliderComponent::CreateBones");
#endif
#endif
                        const auto collider = _bonesColliders[bone];
                        const auto fixture = world->GetFixtureByCollider(collider);
                        const auto body = world->GetBody(entity->GetComponent<Rigidbody2dComponent>());

                        body->DestroyFixture(fixture);

                        CreateColliderFixture(bone, collider);
                    }

                }
            }
        }
    }
}
