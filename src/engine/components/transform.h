#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "entity.h"
#include "sprite_renderer.h"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>         
#include <glm/gtx/quaternion.hpp>         
#include <glm/gtc/type_ptr.hpp>


class Transform : public Component {

    protected:
        glm::vec3 _position;
        glm::vec3 _eulerRotation;
        glm::vec3 _scale;

    public:
        Transform() = delete;
        Transform(const std::weak_ptr<Entity>& entity) : Component(entity),
        _scale(glm::vec3(1.0))
        {
        }
        ~Transform() override = default;

        void Update() const override;

        virtual glm::mat4 GetModel() const;



        void SetPosition(const glm::vec3& position);
        const glm::vec3 GetPosition() const;

        void SetEulerRotation(const glm::vec3 &euler);
        const glm::vec3 GetEulerRotation() const;

        void SetScale(const glm::vec3& scale);
        const glm::vec3 GetScale() const;
};