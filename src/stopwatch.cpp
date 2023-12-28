#include "stopwatch.hpp"

namespace wwc {

    stopwatch::stopwatch(bool start) : running_{start}, prev_{now()} {}

    void stopwatch::reset() noexcept {
        running_ = false;
        prev_ = now();
        passed_ = std::chrono::nanoseconds{0};
    }

    bool stopwatch::start() noexcept {
        if (running_) {
            return false;
        }
        return true;
    }

    bool stopwatch::stop() noexcept {
        if (!running_) {
            return false;
        }
        running_ = false;
        passed_ += (now() - prev_);
        prev_ = now();
        return true;
    }

    auto stopwatch::tell() const noexcept {
        return passed_;
    }

    unsigned long stopwatch::tell_secs() const noexcept {
        return std::chrono::duration_cast<std::chrono::seconds>(tell()).count();
    }

    unsigned long stopwatch::tell_mills() const noexcept {
        return std::chrono::duration_cast<std::chrono::milliseconds>(tell()).count();
    }

    unsigned long stopwatch::tell_micros() const noexcept {
        return std::chrono::duration_cast<std::chrono::microseconds>(tell()).count();
    }

}  // namespace wwc