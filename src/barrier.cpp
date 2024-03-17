#include "barrier.hpp"

#include <chrono>

using namespace wwc;
using namespace std::chrono;

barrier::barrier(std::size_t size) noexcept : count_{size}, original_count_{size} {}

void barrier::arrive_and_wait() {
    if (count_ <= 0) {
        throw std::logic_error("count is smaller than 0");
    }
    --count_;
    cond_.notify_all();
    std::unique_lock<std::mutex> lk{mu_};
    while (count_ > 0) {
        cond_.wait(lk);
    }
}

bool barrier::arrive_and_wait_for(long timeout_mills) {
    if (count_ <= 0) {
        throw std::logic_error("count is smaller than 0");
    }
    --count_;
    cond_.notify_all();
    std::unique_lock<std::mutex> lk{mu_};
    while (count_ > 0) {
        const auto status = cond_.wait_for(lk, milliseconds(timeout_mills));
        if (status == std::cv_status::timeout) {
            return false;
        }
    }
    return true;
}

bool barrier::reset() noexcept {
    if (count_ != 0) {
        return false;
    }
    count_ = original_count_;
    return true;
}