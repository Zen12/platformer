
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
    explicit RectTransform(const std::weak_ptr<Entity> &entity) : Transform(entity)
    {
        _constrains = std::make_unique<LayoutOptions>();
    }

    template<typename TLayoutOption>
    void AddLayoutOption() const {
        _constrains->AddLayout(std::make_unique<TLayoutOption>());
    }

    template<typename TLayoutOption>
    void AddLayoutOption(const float value) const {
        _constrains->AddLayout(std::make_unique<TLayoutOption>(value));
    }

    template<typename TLayoutOption>
    void AddLayoutOption(const glm::vec2 value) const {
        _constrains->AddLayout(std::make_unique<TLayoutOption>(value));
    }

    template<typename TLayoutOption>
    void AddLayoutOption(const float &value, const AlignmentLayout &alignment) const {
        _constrains->AddLayout(std::make_unique<TLayoutOption>(value, alignment));
    }

    void SetParent(const std::weak_ptr<RectTransform> &parent)
    {
        _parent = parent;
    }

    [[nodiscard]] glm::mat4 GetModel() const override;
};