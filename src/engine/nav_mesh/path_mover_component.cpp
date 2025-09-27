#include "path_mover_component.hpp"

void PathMover::Update(const float &deltaTime) {
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