#pragma once

#include <chrono>
#include <exception>

namespace wwc {
    class stopwatch {
      public:
        stopwatch(bool start=false);
        stopwatch(const stopwatch& other) = delete;
        stopwatch(stopwatch&& other) = delete;
        stopwatch& operator=(const stopwatch& other) = delete;
        
        auto now() const noexcept {
            return std::chrono::steady_clock::now();
        }
        void reset() noexcept;
        bool start() noexcept;
        bool stop() noexcept;
        bool is_running() const noexcept {
            return running_;
        }
        auto tell() const noexcept ;
        unsigned long tell_secs() const noexcept;
        unsigned long tell_mills() const noexcept;
        unsigned long tell_micros() const noexcept;
        
      private:
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> prev_;
        std::chrono::nanoseconds passed_ {0};
        bool running_ = false;
    };
}  // namespace wwc
