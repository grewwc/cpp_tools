#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

using namespace std;

namespace wwc {
    template <typename T>
    class MDC {
    public:
        MDC() = delete;

        static void put(std::string key, T val) { m_.emplace(std::move(key), std::move(val)); }

        static std::optional<T> get(const std::string& key) {
            if (m_.find(key) != m_.cend()) {
                return m_.at(key);
            }
            return {};
        }

        static void remove(const std::string& key) { m_.erase(key); }

        static void clear() { m_.clear(); }

    private:
        static inline thread_local std::unordered_map<std::string, T> m_;
    };
}  // namespace wwc