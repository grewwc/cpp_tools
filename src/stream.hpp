#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

namespace wwc {
    
    // just for convinience, NOT efficient!
    template <template <typename, typename...> class Container, class T>
    class Stream {
    public:
        Stream(Container<T>& container) : data_{container} {}

        static Stream<Container, T> from(Container<T>& container) { return Stream(container); }

        Stream<Container, T> map(const std::function<T(const T& )>& func) {
            for(const T& item : data_) {
                next_.push_back(func(item));
            }
            std::swap(data_, next_);
            next_.clear();
            return from(data_);
        }
        
        Stream<Container, T> filter(const std::function<bool(const T& )>& pred) {
            for(const T& item: data_) {
                if (pred(item)) {
                    next_.push_back(item);
                }
            }
            std::swap(data_, next_);
            next_.clear();
            return from(data_);
        }
        
        Container<T> collect() const noexcept {
            return data_;
        }
        
        template<typename To>
        std::unordered_map<To, std::vector<T>> groupby(const std::function<To(const T&)> group_func) const noexcept {
            std::unordered_map<To, std::vector<T>> result;
            for (const T& item : data_) {
                To to = group_func(item);
                result[to].push_back(item);
            }
            return result;
        }

        void for_each(const std::function<void(const T&)>& func) {
            for (const auto& item : data_) {
                func(item);
            }
        }
        

    private:
        Container<T> data_;
        Container<T> next_;
    };
}  // namespace wwc
