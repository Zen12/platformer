#pragma once
#include <string>
#include "node/action/action_serialiazion.hpp"

class ExampleNoopActionSerialization final : public ActionSerialization {
public:
    std::string Label;
    ~ExampleNoopActionSerialization() override = default;
};
