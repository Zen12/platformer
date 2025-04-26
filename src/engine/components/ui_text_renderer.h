#include "../render/font.h"
#include "../render/mesh.h"
#include "../render/shader.h"
#include "../asset/asset_loader.h"
#include "entity.h"

class UiTextRenderer : public Component
{
    private:
        Font _font;
        std::string _text;
        uint32_t VAO, VBO;
        Shader _shader;

    public:
        UiTextRenderer(const std::weak_ptr<Entity>& entity);

        //void SetFont(const std::weak_ptr<Font>& font) { _font = font; };
        void SetText(const std::string& text) { _text = text; };

        void Update() const override;
};