#pragma once
#include <string>


class TagComponent {
private:
    std::string _tag;
public:
    explicit TagComponent(const std::string &tag)
        : _tag(tag)
    {}

    const std::string &GetTag() const noexcept { return _tag; }
};
