#pragma once
#include <variant>

namespace fsm {

template<typename... Ts>
struct TypeList {};

template<typename... Lists>
struct Concat;

template<>
struct Concat<> {
    using Type = TypeList<>;
};

template<typename... Ts>
struct Concat<TypeList<Ts...>> {
    using Type = TypeList<Ts...>;
};

template<typename... Ts, typename... Us, typename... Rest>
struct Concat<TypeList<Ts...>, TypeList<Us...>, Rest...> {
    using Type = typename Concat<TypeList<Ts..., Us...>, Rest...>::Type;
};

template<typename... Lists>
using Concat_t = typename Concat<Lists...>::Type;

template<typename T>
struct ToVariant;

template<typename... Ts>
struct ToVariant<TypeList<Ts...>> {
    using Type = std::variant<Ts...>;
};

template<typename T>
using ToVariant_t = typename ToVariant<T>::Type;

}
