#pragma once
#include <string>
#include "action_serialization.hpp"

class LogActionSerialization final : public ActionSerialization {
public:
    std::string Message;

    ~LogActionSerialization() override = default;
};
