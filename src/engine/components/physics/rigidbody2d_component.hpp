#pragma once
#include "../entity.hpp"


class Rigidbody2dComponent final : public Component {

public:
    explicit Rigidbody2dComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {

    }


    void Update([[maybe_unused]] const float& deltaTime) override
    {
        /*
        const auto position = _body->GetPosition();
        _entity.lock()->GetComponent<Transform>().lock()->SetPosition(
            position.x, position.y);
            */
    }
};
