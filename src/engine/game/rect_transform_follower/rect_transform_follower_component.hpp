#pragma once

#include "../../components/entity.hpp"
#include "../../renderer/transform/transform_component.hpp"
#include "../../ui/rect_transform/rect_transform_component.hpp"
#include "../../renderer/camera/camera_component.hpp"


class RectTransformFollower final: public Component {
private:
    std::weak_ptr<Transform> _target;
    std::weak_ptr<RectTransform> _transform;
    std::weak_ptr<CameraComponent> _camera;

    glm::vec2 _offset;

public:
    explicit RectTransformFollower(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

        if (const auto tr = _transform.lock()){
            if (const auto target = _target.lock()) {
                if (const auto camera = _camera.lock()) {
                    auto screen = camera->WorldToScreenPoint(target->GetPosition());
                    screen.x += _offset.x;
                    screen.y += _offset.y;

                    tr->SetPosition(screen);
                }
            }
        }
    }

    void SetFollowTarget(const std::weak_ptr<Transform> &target) noexcept {
        _target = target;
    }

    void SetTransform(const std::weak_ptr<RectTransform> &transform) noexcept {
        _transform = transform;
    }

    void SetCamera(const std::weak_ptr<CameraComponent> &camera) noexcept {
        _camera = camera;
    }

    void SetOffset(const glm::vec2 &offset) noexcept {
        _offset = offset;
    }
};