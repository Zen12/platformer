#pragma once
#include "../../../entity/entity.hpp"
#include "../../../physics/physics_world.hpp"
#include "../character_animation/character_animation_component.hpp"

struct ShootResult {
    bool IsHit = false;
    std::weak_ptr<Rigidbody2dComponent> Rigidbody;
    glm::vec3 Point = glm::vec3(0.0f);
    glm::vec3 Normal = glm::vec3(0.0f);
    glm::vec3 StartPosition = glm::vec3(0.0f);
    glm::vec3 Direction = glm::vec3(0.0f);
};

class ShootComponent final : public Component {
private:
    std::weak_ptr<PhysicsWorld> _world{};
    std::weak_ptr<CharacterAnimationComponent> _animation;
    std::string _boneName = "gun-tip";
    float _raycastDistance = 1000.0f;

public:
    explicit ShootComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update(const float &deltaTime) override;
    [[nodiscard]] ShootResult Shoot(const glm::vec3 &lookAt) const;

    void SetWorld(const std::weak_ptr<PhysicsWorld> &world) { _world = world; }
    void SetAnimation(const std::weak_ptr<CharacterAnimationComponent> &animation) { _animation = animation; }
    void SetBoneName(const std::string &boneName) { _boneName = boneName; }
    void SetRaycastDistance(const float distance) { _raycastDistance = distance; }

};