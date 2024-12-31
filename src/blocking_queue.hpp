#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

#include "hierachy_mutex.hpp"

namespace wwc {

    template <typename T>
    class blocking_queue {
    public:
        explicit blocking_queue(const size_t sz) noexcept : sz_{sz}, dq_{std::deque<T>(0)} {}

        bool push(const T val, const size_t timeout_mills = 0) noexcept {
            std::unique_lock<wwc::hierachy_mutex> lk(mu_);
            while (true) {
                const auto sz = dq_.size();
                if (sz == sz_) {
                    if (timeout_mills == 0) {
                        push_cond_.wait(lk);
                    } else {
                        const std::cv_status status = push_cond_.wait_for(lk, std::chrono::milliseconds(timeout_mills));
                        if (status == std::cv_status::timeout) {
                            return false;
                        }
                    }
                } else {
                    dq_.push_back(std::move(val));
                    pop_cond_.notify_one();
                    break;
                }
            }
            return true;
        }

        bool try_push(const T val) noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk{mu_};
            if (dq_.size() == sz_) {
                return false;
            }
            dq_.push_back(std::move(val));
            pop_cond_.notify_one();
            return true;
        }

        bool pop(size_t timeout_mills = 0) noexcept {
            std::unique_lock<wwc::hierachy_mutex> lk{mu_};
            while (true) {
                if (dq_.empty()) {
                    if (timeout_mills == 0) {
                        pop_cond_.wait(lk);
                    } else {
                        const std::cv_status status = pop_cond_.wait_for(lk, std::chrono::milliseconds(timeout_mills));
                        if (status == std::cv_status::timeout) {
                            return false;
                        }
                    }

                } else {
                    dq_.pop_front();
                    push_cond_.notify_one();
                    break;
                }
            }
            return true;
        }

        bool try_pop() noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk{mu_};
            if (dq_.empty()) {
                return false;
            }
            dq_.pop_front();
            push_cond_.notify_one();
            return true;
        }

        std::optional<T> get(size_t timeout_mills = 0) noexcept {
            std::unique_lock<wwc::hierachy_mutex> lk{mu_};
            while (true) {
                if (dq_.empty()) {
                    if (timeout_mills == 0) {
                        pop_cond_.wait(lk);
                    } else {
                        const std::cv_status status = pop_cond_.wait_for(lk, std::chrono::milliseconds(timeout_mills));
                        if (status == std::cv_status::timeout) {
                            return {};
                        }
                    }

                } else {
                    T data = dq_.front();
                    dq_.pop_front();
                    push_cond_.notify_one();
                    return std::optional{std::move(data)};
                }
            }
            return {};
        }

        std::optional<T> try_get() noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk{mu_};
            if (dq_.empty()) {
                return {};
            }
            T data = dq_.front();
            dq_.pop_front();
            push_cond_.notify_one();
            return std::optional<T>{std::move(data)};
        }

        bool empty() const noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk{mu_};
            return dq_.empty();
        }

        bool size() const noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk{mu_};
            return dq_.empty();
        }
        
        void clear() noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk(mu_);
            dq_.clear();
        }

    private:
        std::deque<T> dq_;
        const size_t sz_;
        std::condition_variable_any push_cond_;
        std::condition_variable_any pop_cond_;
        wwc::hierachy_mutex mu_{100};
    };
}  // namespace wwc