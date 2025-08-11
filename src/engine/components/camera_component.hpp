#pragma once

#include "../render/camera.hpp"
#include "entity.hpp"
#include <glm/glm.hpp>

class CameraComponent final : public Component
{
private:
    Camera _camera{};

public:
    explicit CameraComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {
    }

    void SetCamera(const Camera &camera)
    {
        _camera = camera;
    }

    [[nodiscard]] glm::mat4 GetProjection() const {
        return _camera.GetProjection();
    }

    void Update([[maybe_unused]] const float& deltaTime) override
    {
    }
};
/*

struct CameraComponentSerialization final : public ComponentSerialization
{
    float aspectPower{};
    bool isPerspective{};

    [[nodiscard]] std::string getType() const override { return "camera"; }
};

class CameraComponentFactor final : public ComponentFactory<CameraComponentSerialization>{

public:
    explicit CameraComponentFactor(const std::weak_ptr<Window> &window)
        : ComponentFactory<CameraComponentSerialization>(window) {
    }

protected:
    void SetComponentImpl(const CameraComponentSerialization &serialization, Component *component) override {
        auto *camera = dynamic_cast<CameraComponent *>(component);
        camera->SetCamera(Camera{serialization.aspectPower, false, serialization.isPerspective, _window});
    }
};

namespace YAML
{
    template <>
    struct convert<CameraComponentSerialization>
    {
        static bool decode(const Node &node, CameraComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.aspectPower = map["aspectPower"].as<float>();
            rhs.isPerspective = map["isPerspective"].as<bool>();
            return true;
        }
    };

} // namespace YAML

*/