#include "countdown_latch.hpp"

#include <chrono>

countdown_latch::countdown_latch(std::size_t size) noexcept : count_{size} {}

void countdown_latch::count_down() {
    if (count_ <= 0) {
        throw std::logic_error("count is smaller than 0");
    }
    --count_;
    cond_.notify_all();
}

void countdown_latch::wait() noexcept {
    std::unique_lock<std::mutex> lk{mu_};
    while (count_ > 0) {
        cond_.wait(lk);
    }
}

bool countdown_latch::wait_for(long timeout_mills) noexcept {
    std::unique_lock<std::mutex> lk{mu_};
    while (count_ > 0) {
        const std::cv_status status = cond_.wait_for(lk, std::chrono::milliseconds(timeout_mills));
        if (status == std::cv_status::timeout) {
            return false;
        }
    }
    return true;
}
