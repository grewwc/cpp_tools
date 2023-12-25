#include "stopwatch.hpp"

namespace wwc {

    stopwatch::stopwatch(bool start) : prev_{now()}, running_{start} {}

    bool stopwatch::start() noexcept {
        if (running_) {
            return false;
        }
        running_ = true;
        prev_ = now();
        return true;
    }

    bool stopwatch::stop() noexcept {
        if (!running_) {
            return false;
        }
        running_ = false;
    }

    auto stopwatch::tell() const noexcept {
        return now() - prev_;
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