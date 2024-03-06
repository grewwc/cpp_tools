#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "algorithm.hpp"

namespace wwc {

    // just for convinience, NOT efficient!
    template <template <typename, typename...> class Container, class T>
    class Stream {
    public:
        Stream(Container<T>& container) : data_{container} {}

        static Stream<Container, T> from(Container<T>& container) { return Stream(container); }
        static Stream<Container, T> from(Container<T>&& container) { return Stream(std::move(container)); }

        Stream<Container, T> map(const std::function<T(const T&)>& func) {
            for (const T& item : data_) {
                next_.push_back(func(item));
            }
            std::swap(data_, next_);
            next_.clear();
            return from(std::move(data_));
        }

        Stream<Container, T> filter(const std::function<bool(const T&)>& pred) {
            for (const T& item : data_) {
                if (pred(item)) {
                    next_.push_back(item);
                }
            }
            std::swap(data_, next_);
            next_.clear();
            return from(std::move(data_));
        }

        Container<T> collect() const noexcept { return std::move(data_); }

        template <typename To>
        std::unordered_map<To, std::vector<T>> groupby(const std::function<To(const T&)> group_func) const noexcept {
            std::unordered_map<To, std::vector<T>> result;
            for (const T& item : data_) {
                To to = group_func(item);
                result[to].push_back(item);
            }
            return result;
        }

        Stream<Container, T> distinct() {
            std::unordered_map<T, size_t> m;
            m.reserve(std::size(data_)/2);
            std::for_each(std::cbegin(data_), std::cend(data_), [&m](const T& item) mutable {
                m[item]++;
            });
            std::copy_if(std::cbegin(data_), std::cend(data_), std::back_inserter(next_), [&m](const T& item){return m.at(item)==1;});
            std::swap(next_, data_);
            next_.clear();
            return from(std::move(data_));
        }

        Stream<Container, T> sort() {
            wwc::quick_sort(data_);
            return from(std::move(data_));
        }

        void for_each(const std::function<void(const T&)>& func) {
            for (const auto& item : data_) {
                func(item);
            }
        }

    private:
        Container<T> data_;
        std::vector<T> next_;

        Stream(Container<T>&& container): data_(std::move(container)){}
    };
}  // namespace wwc
