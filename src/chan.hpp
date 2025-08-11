#pragma once
#include <algorithm>
#include <atomic>
#include <thread>
#include "blocking_queue.hpp"
#include "src/jthread.hpp"

namespace wwc {
    template <typename T>
    class chan {
    public:
        explicit chan(size_t size = 0) : sz_{size}, q_{size == 0 ? 1 : size} {
        }
        chan(const chan<T>& other) = delete;
        chan(chan<T>&& other) : sz_{other.sz_}, q_{std::move(other.q_)}, closed_{other.closed_.load()} {
        }
        chan<T>& operator=(const chan<T>& other) = delete;
        chan<T>& operator=(chan<T>&& other) = delete;
        ~chan() = default;

        void send(T val, long timeout_mills = -1) {
            if (closed_) {
                throw std::logic_error("channel has been closed");
            }
            if (send_callback_ != nullptr) {
                send_callback_();
            }
            q_.push(std::move(val), timeout_mills);
            if (sz_ == 0) {
                blocking_q_.push(nullptr);
                blocking_q_.push(nullptr);
            }
            if (send_callback_ != nullptr) {
                send_callback_();
            }
        }

        std::optional<T> recv(long timeout_mills = -1) {
            if (closed_) {
                return {};
            }
            auto&& res = q_.get(timeout_mills);
            if (sz_ == 0) {
                blocking_q_.get();
                blocking_q_.get();
            }
            return std::move(res);
        }

        size_t size() const noexcept {
            return sz_;
        }

        void close() {
            if (closed_) {
                return;
            }
            std::lock_guard<std::mutex> lk{mu_};
            q_.clear();
            blocking_q_.clear();
            q_.end_flag_ = true;
            blocking_q_.end_flag_ = true;
            q_.pop_cond_.notify_all();
            q_.push_cond_.notify_all();
            blocking_q_.pop_cond_.notify_all();
            blocking_q_.push_cond_.notify_all();
            if (close_cond_) {
                close_cond_->notify_all();
            }
            closed_ = true;
        }

        bool is_closed() const noexcept {
            return closed_;
        }

        template <typename FirstChan, typename... Chan>
        static std::shared_ptr<chan<T>> select(FirstChan* first, Chan*... others) {
            constexpr size_t sz = sizeof...(others);
            if constexpr (sz == 0) {
                return nullptr;
            }
            if constexpr (sz == 1) {
                return std::shared_ptr<chan<T>>(static_cast<chan<T>*>(first));
            }
            auto merged_others = chan<T>::select(others...);
            auto res = chan<T>::select(first, merged_others.get());
            res->children_.push_back(merged_others);
            return res;
        }

        static std::shared_ptr<chan<T>> select(chan<T>* ch1, chan<T>* ch2) {
            auto result = std::make_shared<chan<T>>();
            auto mu = std::make_shared<std::mutex>();
            auto cond = std::make_shared<std::condition_variable>();
            ch1->register_send_callback([=]() {
                cond->notify_all();
            });
            ch2->register_send_callback([=]() {
                cond->notify_all();
            });
            ch1->close_cond_ = cond;
            ch2->close_cond_ = cond;
            const auto push_data = [=]() {
                bool closed[2] = {};
                chan<T>* vec[2] = {ch1, ch2};
                while (true) {
                    int ready_cnt = 0;
                    for (int i = 0; i < 2; i++) {
                        auto ch = vec[i];
                        if (closed[i]) {
                            continue;
                        }
                        if (ch->is_closed()) {
                            closed[i] = true;
                            continue;
                        }
                        while (ch->readable()) {
                            ready_cnt++;
                            auto&& curr_value = ch->recv();
                            // std::cout << "readable value: " << curr_value.value() << std::endl;
                            if (!curr_value.has_value()) {
                                // std::cout << "what: " << ch->q_.dq_.front() << std::endl;
                                continue;
                            }
                            result->send(std::move(curr_value.value()));
                        }
                    }
                    // all closed
                    if (closed[0] && closed[1]) {
                        // std::cout << "closed \n";
                        result->close();
                        return;
                    }
                    if (ready_cnt == 0) {
                        std::unique_lock<std::mutex> lk{*mu};
                        cond->wait(lk);
                    }
                }
            };
            result->ptr_ = std::make_unique<wwc::jthread<std::function<void(void)>>>(std::move(push_data));
            return result;
        }

    private:
        bool readable() const noexcept {
            if (closed_) {
                return false;
            }
            return !q_.empty();
        }

    private:
        void register_send_callback(std::function<void()> send_callback) {
            send_callback_ = send_callback;
        }

    private:
        blocking_queue<T> q_;
        blocking_queue<T*> blocking_q_{1};
        const size_t sz_;

        std::atomic<bool> finished_send_ = false;
        std::atomic<bool> closed_ = false;
        std::function<void()> send_callback_ = nullptr;
        std::shared_ptr<std::condition_variable> close_cond_ = nullptr;
        std::unique_ptr<wwc::jthread<std::function<void(void)>>> ptr_ = nullptr;
        std::vector<std::shared_ptr<chan<T>>> children_;
        std::mutex mu_;
    };
}  // namespace wwc