#pragma once
#include "guid.hpp"
#include <tuple>

class Shader;

template <typename T>
struct ResourceKey { using Type = Guid; };

template <>
struct ResourceKey<Shader> { using Type = std::tuple<Guid, Guid>; };
