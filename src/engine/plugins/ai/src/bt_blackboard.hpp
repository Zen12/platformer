#pragma once
#include <any>
#include <typeindex>
#include <unordered_map>

class BTBlackboard {
    std::unordered_map<std::type_index, std::any> _data;

public:
    template<typename T>
    void Set(T value) {
        _data[std::type_index(typeid(T))] = std::move(value);
    }

    template<typename T>
    const T& Get() const {
        return std::any_cast<const T&>(_data.at(std::type_index(typeid(T))));
    }
};
