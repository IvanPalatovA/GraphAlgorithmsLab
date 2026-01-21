#ifndef TUPLEPRINTER_H
#define TUPLEPRINTER_H

#include <utility>
#include <tuple>
#include <iostream>

template <typename Tuple, std::size_t... Is>
void print_tuple_impl(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template <typename... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t) {
    os << "(";
    print_tuple_impl(t, std::index_sequence_for<Args...>{});
    os << ")";
    return os;
}

#endif