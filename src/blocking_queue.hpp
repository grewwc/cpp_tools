#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>

#include "hierachy_mutex.hpp"
#include "src/semaphore.hpp"

namespace wwc {

    template <typename T>
    class chan;

    template <typename T>
    class blocking_queue {
    public:
        friend class chan<T>;

        explicit blocking_queue(const size_t sz) noexcept : sz_{sz}, dq_{std::deque<T>(0)} {
        }
        blocking_queue(blocking_queue<T>&& other) : sz_{other.sz_}, dq_{std::move(other.dq_)} {
        }

        blocking_queue<T>& operator=(blocking_queue<T>&& other) = delete;

        bool push(const T val, long timeout_mills = -1) noexcept {
            if (sz_ == 0) {
                get_sem_.release();
                push_sem_.acquire();
            }
            std::unique_lock<wwc::hierachy_mutex> lk(mu_);
            while (!end_flag_) {
                const auto sz = dq_.size();
                const auto cap = sz_ == 0 ? 1 : sz_;
                if (sz == cap) {
                    if (timeout_mills < 0) {
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

        std::optional<T> get(long timeout_mills = -1) noexcept {
            if (sz_ == 0) {
                push_sem_.release();
                get_sem_.acquire();
            }
            std::unique_lock<wwc::hierachy_mutex> lk{mu_};
            while (!end_flag_) {
                if (dq_.empty()) {
                    if (timeout_mills < 0) {
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
            return dq_.size();
        }

        bool capacity() const noexcept {
            std::lock_guard<wwc::hierachy_mutex> lk{mu_};
            return sz_;
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
        mutable wwc::hierachy_mutex mu_{100};
        std::atomic<bool> end_flag_ = false;
        wwc::semaphore push_sem_;
        wwc::semaphore get_sem_;
    };
}  // namespace wwc