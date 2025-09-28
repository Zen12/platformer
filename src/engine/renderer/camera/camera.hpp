#pragma once

#include "../../system/window.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    float _aspectPower{};
    std::weak_ptr<Window> _window{};
    bool _isPerspective{};
    bool _isUi{};

public:
    Camera() = default;

    Camera(
        const float &aspectPower,
        const bool &isUi,
        const bool &isPerspective,
        const std::weak_ptr<Window> &window)
            : _aspectPower(aspectPower), _window(window), _isPerspective(isPerspective), _isUi(isUi)
    {
    }

    static Camera Ortho(const std::weak_ptr<Window> &window)
    {
        constexpr float orthoPower = 8.0f;
        return {orthoPower, false, false, window};
    }

    static Camera OrthoUI(const std::weak_ptr<Window> &window)
    {
        constexpr float orthoPower = 8.0f;
        return {orthoPower, true, false, window};
    }

    static Camera Perspective(const std::weak_ptr<Window> &window)
    {
        constexpr float perspectivePower = 45;
        return {perspectivePower, false, true, window};
    }

    [[nodiscard]] glm::mat4 GetProjection() const
    {
        if (const auto w = _window.lock())
        {
            if (_isPerspective)
            {
                const float ration = static_cast<float>(w->GetWidth()) / static_cast<float>(w->GetHeight());
                return glm::perspective(glm::radians(_aspectPower), ration, -0.1f, 100.0f);
            }
            else if (_isUi)
            {
                const float width = static_cast<float>(w->GetWidth()) / _aspectPower;
                const float height = static_cast<float>(w->GetHeight()) / _aspectPower;

                return glm::ortho(0.0f, width, 0.0f, height, -1.0f, 100.0f);
            } else {
                const float width = static_cast<float>(w->GetWidth()) / _aspectPower;
                const float height = static_cast<float>(w->GetHeight()) / _aspectPower;

                const float halfWidth = width * 0.5f;
                const float halfHeight = height * 0.5f;

                return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 100.0f);
            }
        }

        return glm::mat4(1.0);
    }
};