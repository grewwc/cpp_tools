#pragma once

#include <thread>

namespace wwc {
    template <typename Fn, typename... Args>
    class jthread : public std::thread {
    public:
        explicit jthread() = default;
        jthread(const jthread&) = delete;
        jthread(jthread&&) = default;
        jthread(Fn&& fn, Args... args) : std::thread{std::forward<Fn>(fn), std::forward<Args>(args)...} {}

        ~jthread() {
            if (joinable()) {
                join();
            }
        }
    };
}  // namespace wwc