#pragma once

class Entity;

class Component
{
public:
    explicit Component(const std::weak_ptr<Entity>& entity)
        : _entity(entity) {}

    [[nodiscard]] std::weak_ptr<Entity> GetEntity() const { return _entity; }

    virtual void Update([[maybe_unused]] const float& deltaTime) = 0;
    virtual ~Component() = default;

protected:
    std::weak_ptr<Entity> _entity;
};