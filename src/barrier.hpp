#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <atomic>

namespace wwc {

    class barrier {
    public:
        explicit barrier(std::size_t) noexcept;
        void arrive_and_wait();
        bool arrive_and_wait_for(long timeout_mills);
        bool reset() noexcept;

    private:
        std::mutex mu_;
        std::condition_variable cond_;
        std::atomic<std::size_t> count_;
        const std::size_t original_count_;
    };
}  // namespace wwc