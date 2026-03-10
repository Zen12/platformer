#pragma once
#include "resource_key.hpp"
#include <tuple>

class Shader;

template <>
struct ResourceKey<Shader> { using Type = std::tuple<Guid, Guid>; };
