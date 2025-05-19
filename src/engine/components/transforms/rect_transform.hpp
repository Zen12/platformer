
#pragma once

#include "transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "layout_options.hpp"
#include <iostream>
#include <memory>

class RectTransform : public Transform
{

private:
    std::unique_ptr<LayoutOptions> _constrains;
    std::weak_ptr<RectTransform> _parent;

public:
    RectTransform(const std::weak_ptr<Entity> &entity) : Transform(entity)
    {
        _constrains = std::make_unique<LayoutOptions>();
    };

    void AddLayoutOption(std::unique_ptr<LayoutOption> layout)
    {
        _constrains->AddLayout(std::move(layout));
    }

    void SetParent(const std::weak_ptr<RectTransform> &parent)
    {
        _parent = parent;
    }

    [[nodiscard]] glm::mat4 GetModel() const override;
};