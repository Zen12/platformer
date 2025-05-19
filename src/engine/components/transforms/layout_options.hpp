
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <glm/glm.hpp>
#include <iostream>

enum AlligmentLayout
{
    StartBorder,
    EndBorder
};

class LayoutOption
{
public:
    LayoutOption() = default;

    virtual glm::mat4 UpdateMatrix(const float &width, const float &height, const glm::mat4 &mat) const = 0;
    virtual ~LayoutOption() = default;
};

class CenterXLayoutOption : public LayoutOption
{
public:
    [[nodiscard]] glm::mat4 UpdateMatrix(
        const float &width,
        [[maybe_unused]] const float &height,
        [[maybe_unused]] const glm::mat4 &model)
        const override
    {
        return glm::translate(model, glm::vec3(width / 2, 0, 0));
    }
};

class PixelXLayoutOption : public LayoutOption
{
private:
    const float _value;
    const AlligmentLayout _alligment;

public:
    PixelXLayoutOption(const float &value, const AlligmentLayout &allignment)
        : _value(value), _alligment(allignment) {};

    [[nodiscard]] glm::mat4 UpdateMatrix(
        const float &width,
        [[maybe_unused]] const float &height,
        const glm::mat4 &model) const override
    {
        switch (_alligment)
        {
        case AlligmentLayout::StartBorder:
            return glm::translate(model, glm::vec3(0 + _value, 0, 0));
        case AlligmentLayout::EndBorder:
            return glm::translate(model, glm::vec3(width - _value, 0, 0));
        default:
            break;
        }

        return model;
    }
};

class PivotLayoutOption : public LayoutOption
{
private:
    const glm::vec2 _pivot;

public:
    PivotLayoutOption(const glm::vec2 pivot) : _pivot(pivot) {};

    [[nodiscard]] glm::mat4 UpdateMatrix(
        [[maybe_unused]] const float &width,
        [[maybe_unused]] const float &height,
        const glm::mat4 &model)
        const override
    {
        return glm::translate(model, glm::vec3(-_pivot.x, -_pivot.y, 0.0));
    }
};

class CenterYLayoutOption : public LayoutOption
{
public:
    [[nodiscard]] glm::mat4 UpdateMatrix(
        [[maybe_unused]] const float &width,
        const float &height, const glm::mat4 &model)
        const override
    {
        return glm::translate(model, glm::vec3(0, height / 2, 0));
    }
};

class PixelHeightLayoutOption : public LayoutOption
{
private:
    const float _valuePixels;

public:
    PixelHeightLayoutOption(const float &valuePixels) : LayoutOption(),
                                                        _valuePixels(valuePixels)
    {
    }

    [[nodiscard]] glm::mat4 UpdateMatrix(
        [[maybe_unused]] const float &width,
        [[maybe_unused]] const float &height, const glm::mat4 &model) const override
    {
        return glm::scale(model, glm::vec3(1, _valuePixels, 1));
    }
};

class PixelWidthLayoutOption : public LayoutOption
{
private:
    const float _value;

public:
    PixelWidthLayoutOption(const float &value) : LayoutOption(), _value(value)
    {
    }

    [[nodiscard]] glm::mat4 UpdateMatrix(
        [[maybe_unused]] const float &width,
        [[maybe_unused]] const float &height,
        const glm::mat4 &model) const override
    {
        return glm::scale(model, glm::vec3(_value, 1, 1));
    }
};

class LayoutOptions
{

private:
    std::vector<std::unique_ptr<LayoutOption>> _layouts;

public:
    void AddLayout(std::unique_ptr<LayoutOption> value)
    {
        _layouts.push_back(std::move(value));
    }

    [[nodiscard]] glm::mat4 Build(const glm::mat4 &model, const float &width, const float &height) const
    {
        glm::mat4 mat = model;
        for (const auto &layout : _layouts)
        {
            mat = layout->UpdateMatrix(width, height, mat);
        }

        return mat;
    }
};
