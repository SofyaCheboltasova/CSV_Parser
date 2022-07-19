#pragma once
#include <iostream>
#include <tuple>
#include <string>

// Subtask #1 - print tuple

template<class Tuple, size_t N>
struct TuplePrinter {
    static void print(std::ostream& out, Tuple const& tuple) {
        TuplePrinter<Tuple, N - 1>::print(out, tuple);
        out << ", " << std::get<N - 1>(tuple);
    }
};

template<class Tuple>
struct TuplePrinter<Tuple, 1> {
    static void print(std::ostream &out, Tuple const& tuple) {
        out << std::get<0>(tuple);
    }
};

template<typename... Args>
std::ostream& operator<<(std::ostream& out, std::tuple<Args...> const& tuple) {
    std::cout << "(";
    TuplePrinter<decltype(tuple), sizeof...(Args)>::print(out, tuple);
    std::cout << ")";
}