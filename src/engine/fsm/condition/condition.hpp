#pragma once
#include <string>
#include <unordered_map>

class Condition {
protected:
    std::shared_ptr<std::unordered_map<std::string, std::variant<bool, int, float>>> _dictionary;

public:
    virtual ~Condition() = default;

    std::string Type;
    std::string Guid;

    [[nodiscard]] virtual bool IsSuccess() = 0;

    void SetDictionary(std::shared_ptr<std::unordered_map<std::string, std::variant<bool, int, float>>> dictionary) noexcept {
        _dictionary = dictionary;
    }
};
