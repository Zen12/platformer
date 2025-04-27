#pragma once

#include "../system/window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera
{
    private:
        float _aspectPower;
        std::weak_ptr<Window> _window;
        bool _isPespective;

    public:
        Camera() = default;

        Camera(const float& aspectPower, const bool& isPespective, const std::weak_ptr<Window>& window) :
            _aspectPower(aspectPower),
            _isPespective(isPespective),
            _window(window)
        {
        }


        static Camera Ortho(std::weak_ptr<Window> window)
        {
            constexpr float orthoPower = 8.0f;
            return Camera(orthoPower, false, window);
        }

        static Camera Perspective(std::weak_ptr<Window> window)
        {
            constexpr float perspectivePower = 45;
            return Camera(perspectivePower, true, window);
        }

        const glm::mat4 GetProjection() const
        {
            if (auto w = _window.lock())
            {
                if (_isPespective)
                { 
                    float ration = (float)w->GetWidth()/(float) w->GetHeight();
                    return glm::perspective(glm::radians(_aspectPower), ration, 0.1f, 100.0f);
                } else {
                    const float width = ((float)w->GetWidth()) / _aspectPower; 
                    const float height = ((float) w->GetHeight()) / _aspectPower;

                    return glm::ortho(0.0f, width , 0.0f, height, -0.1f, 100.0f);
                }
            }

            return glm::mat4(1.0);
        }

};