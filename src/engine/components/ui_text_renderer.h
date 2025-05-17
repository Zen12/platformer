#pragma once

#include "../render/font.h"
#include "../render/mesh.h"
#include "../render/material.h"
#include "../asset/asset_loader.h"
#include "../components/transform.h"
#include "../components/rect_transform.h"
#include "entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>         
#include <glm/gtx/quaternion.hpp>         
#include <glm/gtc/type_ptr.hpp>


class UiTextRenderer : public Component
{
    private:
        std::string _text;
        uint32_t VAO, VBO;
        float _fontSize = 1;
        std::weak_ptr<Material> _material;

    public:
        UiTextRenderer(const std::weak_ptr<Entity>& entity);

        void SetText(const std::string& text) 
        { 
            _text = text; 
        };
        void SetMaterial(const std::weak_ptr<Material>& material)
        {
            _material = material;
        }

        void Update() const override;
        void Render(const glm::mat4& projection, const uint16_t& width, const uint16_t& height);

        int32_t GetShaderId() const { return _material.lock()->GetShaderId();}

    private:
        void Bind() ;
};