
#pragma once

#include "transform.h"

#include <glm/glm.hpp>



class RectTransform : public Transform
{

    private:
        glm::vec4 _rect;

    public:
        RectTransform(const std::weak_ptr<Entity>& entity) : Transform(entity)
        {};

        /// @brief xy - position, zw - delta
        /// @param rect 
        void SetRect(const glm::vec4& rect) {_rect = rect;}
        const glm::vec4 GetAnchoredPosion() const;
        const std::array<glm::vec2, 4> GetCorners() const;

        glm::mat4 GetModel() const;
};