#include <string>
#include <iostream>
#include <iterator>

namespace wwc {

    template <typename T>
    concept ToString = requires(const T& s) { std::to_string(s); };

    template <typename T>
    concept Printable = requires(std::ostream& os, T&& arg) { os << arg; };

    template <typename... Args>
    concept AllPrintable = (Printable<Args> && ...);

    template <template <typename...> typename Container, typename T>
    concept Iterable = requires(const Container<T>& container) {
        requires(ToString<T>);

        std::cbegin(container) || std::begin(container);
        std::cend(container) || std::end(container);
    };

}
