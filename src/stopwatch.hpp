#pragma once

#include <chrono>
#include <exception>

namespace wwc {
    class stopwatch {
    public:
        stopwatch(bool start = false) : running_{start}, prev_{std::chrono::steady_clock::now()} {}
        stopwatch(const stopwatch& other) = delete;
        stopwatch(stopwatch&& other) = delete;
        stopwatch& operator=(const stopwatch& other) = delete;

        auto now() const noexcept { return std::chrono::steady_clock::now(); }
        void reset() noexcept {
            running_ = false;
            prev_ = now();
            passed_ = std::chrono::nanoseconds{0};
        }
        bool start() noexcept {
            if (running_) {
                return false;
            }
            running_ = true;
            return true;
        }
        bool stop() noexcept {
            if (!running_) {
                return false;
            }
            running_ = false;
            passed_ += (now() - prev_);
            prev_ = now();
            return true;
        }
        bool is_running() const noexcept { return running_; }
        auto tell() const noexcept {
            if (running_) {
                return passed_ + (now() - prev_);
            }
            return passed_;
        }

        unsigned long tell_secs() const noexcept { return std::chrono::duration_cast<std::chrono::seconds>(tell()).count(); }
        unsigned long tell_mills() const noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(tell()).count(); }
        unsigned long tell_micros() const noexcept { return std::chrono::duration_cast<std::chrono::microseconds>(tell()).count(); }

    private:
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> prev_;
        std::chrono::nanoseconds passed_{0};
        bool running_ = false;
    };
}  // namespace wwc
