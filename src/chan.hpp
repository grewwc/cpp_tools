#pragma once
#include <atomic>
#include "blocking_queue.hpp"

namespace wwc {
    template <typename T>
    class chan {
    public:
        explicit chan(size_t size = 0) : sz_{size}, q_{size == 0 ? 1 : size} {
        }
        void send(T val, size_t timeout_mills = 0) {
            if (sz_ > 0) {
                q_.push(std::move(val), timeout_mills);
            } else {
                q_.push(val, timeout_mills);
                q_.push(std::move(val), timeout_mills);
            }
        }
        std::optional<T> recv(size_t timeout_mills = 0) {
            if (sz_ > 0) {
                return q_.get(timeout_mills);
            }
            const auto res = q_.get(timeout_mills);
            q_.pop(timeout_mills);
            return res;
        }

        void close() {
            if (closed_) {
                return;
            }
            q_.clear();
        }

    private:
        blocking_queue<T> q_;
        const size_t sz_;
        std::atomic<bool> closed_;
    };
}  // namespace wwc