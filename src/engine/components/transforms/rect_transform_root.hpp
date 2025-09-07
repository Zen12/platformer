
#pragma once
#include "rect_transform.hpp"
#include "../../system/window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RectTransformRoot final : public RectTransform
{
private:
    std::weak_ptr<Window> _window;

public:
    explicit RectTransformRoot(const std::weak_ptr<Window> &window) : RectTransform(std::weak_ptr<Entity>()),
                                                             _window(window) {

                                                             };

    [[nodiscard]] glm::mat4 GetModel() const override
    {
        glm::mat4 model = glm::mat4(1.0);

        if (const auto window = _window.lock())
        {
            model = glm::translate(model, glm::vec3(1.0, 1.0, 1.0));
            model = glm::scale(model, glm::vec3(window->GetWidth(), window->GetHeight(), 1.0));
        }

        return model;
    };
};