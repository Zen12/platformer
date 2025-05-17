
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>         
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

class LayoutOption
{
    public:
    virtual glm::mat4 UpdateMatrix(const float& width, const float& height, const glm::mat4& mat) const
    {
        return mat;
    }
};


class CenterXLayoutOption : public LayoutOption
{
    public:

    glm::mat4 UpdateMatrix(const float& width, const float& height, const glm::mat4& model) const override
    {
        return glm::translate(model, glm::vec3(width / 2, 0, 0));
    }
};

class CenterYLayoutOption : public LayoutOption
{
    public:

    glm::mat4 UpdateMatrix(const float& width, const float& height, const glm::mat4& model) const override
    {
        return glm::translate(model, glm::vec3(0, height / 2, 0));
    }
};

class PixelHeightLayoutOption : public LayoutOption
{
    private:
        float _valuePixels;
    
    public:
        PixelHeightLayoutOption(const float& valuePixels) : LayoutOption(),
             _valuePixels(valuePixels)
        {}

        glm::mat4 UpdateMatrix(const float& width, const float& height, const glm::mat4& model) const override
        {
            return glm::scale(model, glm::vec3(1, _valuePixels, 1));
        }
};

class PixelWidthLayoutOption : public LayoutOption
{
    private:
        float _value;
    
    public:
        PixelWidthLayoutOption(const float& value) : LayoutOption()
            , _value(value)
        {} 

        glm::mat4 UpdateMatrix(const float& width, const float& height, const glm::mat4& model) const override
        {
            return glm::scale(model, glm::vec3(_value, 1, 1));
        }
};



class LayoutOptions
{

    private:
        std::vector<std::unique_ptr<LayoutOption>> layouts;

    public:

        void AddLayout(std::unique_ptr<LayoutOption> value)
        {
            layouts.push_back(std::move(value));
        }

        const glm::mat4 Build(const glm::mat4& model, const float& width, const float& height) const
        {
            glm::mat4 mat = model;
            for (const auto &layout : layouts)
            {
                mat = layout->UpdateMatrix(width, height, mat);
            }

            return mat;
        }

    
};

