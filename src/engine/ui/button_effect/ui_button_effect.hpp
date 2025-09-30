#pragma once

#include "../button/button_component.hpp"
#include "../image/image_renderer_component.hpp"
#include "../../entity/entity.hpp"

#include <glm/glm.hpp>

class UiButtonEffect final : public Component
{
private:
    std::weak_ptr<UiButtonComponent> _button{};
    std::weak_ptr<UiImageRendererComponent> _image{};
    int _subscribeIdClick{-1};
    int _subscribeIdHover{-1};
    int _subscribeIdDeselect{-1};

    glm::vec4 _delected{1.0f};
    glm::vec4 _hover{0.5f, 0.5f, 0.5f, 1.0f};
    glm::vec4 _clicked{1.0f};

public:
    explicit UiButtonEffect(const std::weak_ptr<Entity> &entity);

    void SetTarget(const std::weak_ptr<UiImageRendererComponent> &uiImage) { _image = uiImage; }
    void SetButton(const std::weak_ptr<UiButtonComponent> &button);

    void Update([[maybe_unused]] const float &deltaTime) override;

    ~UiButtonEffect() override;

private:
    void OnClick() const;
    void OnHover() const;
    void OnDeselect() const;
};