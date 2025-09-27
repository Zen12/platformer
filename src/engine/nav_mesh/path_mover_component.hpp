#pragma once
#include "../components/entity.hpp"
#include "../components/transforms/transform.hpp"


class PathMover final : public Component {
private:
    std::vector<glm::vec3> _points;
    float _speed{1};
    size_t indexPoint{0};
    std::weak_ptr<Transform> _transform;
public:
    explicit PathMover(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update(const float &deltaTime) override;

    void SetPoints(const std::vector<glm::vec3> &points) noexcept {
        _points = points;
    }

    void SetSpeed(const float &speed) noexcept {
        _speed = speed;
    }

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }
};