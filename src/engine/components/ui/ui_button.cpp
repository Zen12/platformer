#include "ui_button.hpp"

int UiButton::AddCallback(const std::function<void()> &callback) {
    int id = _nextId++;
    _callbacks.emplace_back(id, callback);
    return id;
}

void UiButton::RemoveCallback(int id) {
    _callbacks.erase(
        std::remove_if(_callbacks.begin(), _callbacks.end(),
                       [id](const auto& pair) { return pair.first == id; }),
        _callbacks.end());
}

