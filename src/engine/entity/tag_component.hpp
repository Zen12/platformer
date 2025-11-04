#pragma once
#include <string>
#include <utility>


class TagComponent {
private:
    std::string _tag;
public:
    explicit TagComponent(std::string tag)
        : _tag(std::move(tag))
    {}

    [[nodiscard]] const std::string &GetTag() const noexcept { return _tag; }
};
