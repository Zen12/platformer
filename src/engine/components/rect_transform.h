
#pragma once

#include "transform.h"

#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>         
#include <glm/gtx/quaternion.hpp>         
#include <glm/gtc/type_ptr.hpp>



class RectTransform : public Transform
{

    private:
        glm::vec4 _rect;

    public:
        RectTransform(const std::weak_ptr<Entity>& entity) : Transform(entity)
        {};

        void SetRect(const glm::vec4& rect) {_rect = rect;}
        const glm::vec4 GetAnchoredPosion() const {return _rect;}
        
        glm::mat4 GetModel() const;
};