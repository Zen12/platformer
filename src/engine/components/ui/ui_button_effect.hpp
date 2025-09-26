#pragma once
#include "ui_button.hpp"
#include "../../ui/image/image_renderer_component.hpp"


class UiButtonEffect final : public Component {
private:
    std::weak_ptr<UiButton> _button;
    std::weak_ptr<UiImageRendererComponent> _image;
    int _subscribeIdClick{-1};
    int _subscribeIdHover{-1};
    int _subscribeIdDeselect{-1};

    glm::vec4 _delected{glm::vec4(1.0f)};
    glm::vec4 _hover{glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)};
    glm::vec4 _clicked{glm::vec4(1.0f)};

    void OnClick() const noexcept {
        if (const auto image = _image.lock()) {
            image->SetTint(_clicked);
        }
    }

    void OnHover() const noexcept {
        if (const auto image = _image.lock()) {
            image->SetTint(_hover);
        }
    }

    void OnDeselect() const noexcept {
        if (const auto image = _image.lock()) {
            image->SetTint(_delected);
        }
    }

public:
    explicit UiButtonEffect(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetTarget(const std::weak_ptr<UiImageRendererComponent> &uiImage) noexcept {
        _image = uiImage;
    }

    void SetButton(const std::weak_ptr<UiButton> &button) noexcept {

        _button = button;
        if (const auto b = _button.lock()) {
            _subscribeIdClick = b->AddOnClickCallback([this]() {
                this->OnClick();
            });

            _subscribeIdHover = b->AddOnHoverCallback([this]() {
                this->OnHover();
            });

            _subscribeIdDeselect = b->AddOnDeselectCallback([this]() {
                this->OnDeselect();
            });
        }
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }

    ~UiButtonEffect() override {
        if (const auto b = _button.lock()) {
            b->RemoveOnClickCallback(_subscribeIdClick);
            b->RemoveOnHoverCallback(_subscribeIdHover);
            b->RemoveOnDeselectCallback(_subscribeIdDeselect);
        }
    }

};
