#pragma once

#include "../render/font.h"
#include "../render/mesh.h"
#include "../render/shader.h"
#include "../asset/asset_loader.h"
#include "../components/transform.h"
#include "../components/rect_transform.h"
#include "entity.h"

class UiTextRenderer : public Component
{
    private:
        Font _font;
        std::string _text;
        uint32_t VAO, VBO;
        float _fontSize = 1;

    public:
        UiTextRenderer(const std::weak_ptr<Entity>& entity);

        void SetText(const std::string& text) { _text = text; };

        void Update() const override;
        void Render(const Shader& shader, const uint16_t& width, const uint16_t& height);

    private:
        void Bind() ;
};