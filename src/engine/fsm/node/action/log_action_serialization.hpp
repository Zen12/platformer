#pragma once
#include <string>
#include "action_serialiazion.hpp"

class LogActionSerialization final : public ActionSerialization {
public:
    std::string Message;

    ~LogActionSerialization() override = default;
};
