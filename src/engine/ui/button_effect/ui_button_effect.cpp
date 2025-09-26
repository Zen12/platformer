#include "ui_button_effect.hpp"

UiButtonEffect::UiButtonEffect(const std::weak_ptr<Entity> &entity)
    : Component(entity)
{
}

void UiButtonEffect::Update([[maybe_unused]] const float &deltaTime)
{
}

void UiButtonEffect::SetButton(const std::weak_ptr<UiButtonComponent> &button)
{
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

UiButtonEffect::~UiButtonEffect()
{
    if (const auto b = _button.lock()) {
        b->RemoveOnClickCallback(_subscribeIdClick);
        b->RemoveOnHoverCallback(_subscribeIdHover);
        b->RemoveOnDeselectCallback(_subscribeIdDeselect);
    }
}

void UiButtonEffect::OnClick() const
{
    if (const auto image = _image.lock()) {
        image->SetTint(_clicked);
    }
}

void UiButtonEffect::OnHover() const
{
    if (const auto image = _image.lock()) {
        image->SetTint(_hover);
    }
}

void UiButtonEffect::OnDeselect() const
{
    if (const auto image = _image.lock()) {
        image->SetTint(_delected);
    }
}