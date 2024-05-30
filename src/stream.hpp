#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "algorithm.hpp"
namespace wwc {

    // just for convinience, NOT efficient!
    template <template <typename, typename...> class Container, class T>
    class Stream {
    public:
        Stream(Container<T>& container) : data_{container} {}

        static Stream<Container, T> from(Container<T>& container) { return Stream(container); }
        static Stream<Container, T> from(Container<T>&& container) { return Stream(std::move(container)); }

        template <typename U>
        Stream<Container, U> map(const std::function<U(const T&)>& func) {
            std::vector<U> result;
            result.reserve(std::size(data_));
            for (const T& item : data_) {
                result.emplace_back(std::move(func(item)));
            }
            return Stream<Container, U>::from(std::move(result));
        }

        template <typename U>
        Stream<Container, U> flat_map(std::function<U(const T&)> func) {
            std::vector<U> result;
            for (const auto& item : data_) {
                for (const auto& nested_item : item) {
                    result.emplace_back(::std::move(func(nested_item)));
                }
            }
            return Stream<Container, U>::from(std::move(result));
        }

        template <typename U>
        Stream<Container, U> flat_map() {
            std::vector<U> result;
            for (const auto& item : data_) {
                for (const auto& nested_item : item) {
                    result.push_back((U)nested_item);
                }
            }
            return Stream<Container, U>::from(std::move(result));
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
        
        size_t count() const noexcept {
            return data_.size();
        }
        
        T max() const noexcept {
            return *std::max_element(std::cbegin(data_), std::cend(data_));
        }
        
        T min() const noexcept {
            return *std::min_element(std::cbegin(data_), std::cend(data_));
        }

        Stream<Container, T> shuffle() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(std::begin(data_), std::end(data_), std::move(gen));
            return *this;
        }

        Stream<Container, T> first(size_t n) {
            const auto sz = std::size(data_);
            if (sz <= n) {
                return *this;
            }
            for (size_t i = 0; i < n; i++) {
                next_.emplace_back(std::move(data_[i]));
            }
            std::swap(next_, data_);
            next_.clear();
            return from(data_);
        }

        Container<T> collect() const noexcept { return std::move(data_); }

        template <typename Target>
        std::optional<Target> collect(const std::function<Target(const Target&, const T&)> join_func) {
            const auto sz = std::size(data_);
            if (sz == 0) {
                return {};
            }
            Target result = data_[0];
            for (size_t i = 1; i < sz; i++) {
                result = join_func(result, data_[i]);
            }
            return result;
        }

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
            m.reserve(std::size(data_) / 2);
            std::for_each(std::cbegin(data_), std::cend(data_), [&m](const T& item) mutable { m[item]++; });
            std::copy_if(std::cbegin(data_), std::cend(data_), std::back_inserter(next_), [&m](const T& item) { return m.at(item) == 1; });
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

        void for_each_parallel(const std::function<void(const T&)>& func, unsigned int n) {
            BS::thread_pool pool{n};
            for (const auto& item: data_) {
                const auto fu = pool.submit_task([&](){return func(item);});
            }
            pool.wait();
        }
        
    private:
        Container<T> data_;
        std::vector<T> next_;
        Stream(Container<T>&& container) : data_(std::move(container)) {}
    };
}  // namespace wwc
