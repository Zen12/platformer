#pragma once
#include "../components/entity.hpp"
#include "../components/transforms/transform.hpp"


class PathMover final : public Component {
private:
    std::vector<glm::vec3> _points;
    float _speed{1};
    int indexPoint{0};
    std::weak_ptr<Transform> _transform;
public:
    explicit PathMover(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update(const float &deltaTime) override {
        if (const auto &transform = _transform.lock()) {
            const auto &position = transform->GetPosition();
            glm::vec3 dir = (_points.at(indexPoint) - position);
            if (glm::length(dir) < 0.1f) {
                indexPoint++;
                if (indexPoint >= _points.size()) {
                    indexPoint = 0;
                }
                dir = (_points.at(indexPoint) - position);
            }

            const auto finalPos = position + glm::normalize(dir) * (_speed * deltaTime);
            transform->SetPosition(finalPos);
        }
    }

    void SetPoints(const std::vector<glm::vec3> &points) noexcept {
        _points = points;
    }

    void SetSpeed(float speed) noexcept {
        _speed = speed;
    }

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }
};
