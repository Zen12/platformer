#pragma once
#include <unordered_map>

#include "../condition.hpp"


class BoolCheckCondition final : public Condition {

    const std::string _valueName;
    const bool _value;


public:

    explicit BoolCheckCondition(const std::string& valueName, const bool &value)
        : _valueName(valueName), _value(value) {
    }

    [[nodiscard]] bool IsSuccess() override {
        if (!_dictionary) {
            return false; // No dictionary available
        }

        const auto it = _dictionary->find(_valueName);
        if (it == _dictionary->end()) {
            return false; // Key not found
        }

        if (const bool* pValue = std::get_if<bool>(&it->second)) {
            return *pValue == _value; // key exists and it's bool, check value
        }


        return false; // value isn't bool
    }


};
