#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>

namespace wwc {
    class countdown_latch {
    public:
        explicit countdown_latch(std::size_t size) noexcept;
        void count_down();
        void wait() noexcept;
        bool wait_for(long timeout_milss) noexcept;

    private:
        std::atomic<std::size_t> count_;
        std::condition_variable cond_;
        std::mutex mu_;
    };
}  // namespace wwc