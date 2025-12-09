#pragma once
#include <string>

#include "condition/condition.hpp"
#include "node/node_serialization.hpp"

class FsmSerialization final {
public:
  std::string Name{};
  std::string StartNode{};
  std::vector<StateNodeSerialization> Nodes{};
  std::vector<std::unique_ptr<Condition>> Conditions{};
};

