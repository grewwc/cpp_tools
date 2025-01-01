#pragma once
#include <algorithm>
#include <atomic>
#include "BS_thread_pool.hpp"
#include "blocking_queue.hpp"
namespace wwc {
    template <typename T>
    class chan {
    public:
        explicit chan(size_t size = 0) : sz_{size}, q_{size == 0 ? 1 : size}, blocking_q_{1} {
        }
        chan(const chan<T>& other) = delete;
        chan(chan<T>&& other) : sz_{other.sz_}, q_{std::move(other.q_)}, closed_{other.closed_.load()} {
        }
        chan<T>& operator=(const chan<T>& other) = delete;
        chan<T>& operator=(chan<T>&& other) = delete;
        ~chan() = default;

        void send(T val, size_t timeout_mills = -1) {
            if (closed_) {
                throw std::logic_error("channel has been closed");
            }
            q_.push(std::move(val), timeout_mills);
            if (send_callback_ != nullptr) {
                send_callback_();
            }
            if (sz_ == 0) {
                blocking_q_.push(true);
                blocking_q_.push(true);
            }
        }

        std::optional<T> recv(size_t timeout_mills = -1) {
            if (closed_) {
                return {};
            }
            if (sz_ == 0) {
                blocking_q_.get(timeout_mills);
                blocking_q_.get(timeout_mills);
            }
            return q_.get(timeout_mills);
        }

        size_t size() const noexcept {
            return sz_;
        }

        void close() {
            if (closed_) {
                return;
            }
            q_.clear();
            blocking_q_.clear();
            q_.end_flag_ = true;
            q_.pop_cond_.notify_all();
            q_.push_cond_.notify_all();
            if (close_cond_) {
                close_cond_->notify_all();
            }
            closed_ = true;
        }

        bool is_closed() const noexcept {
            return closed_;
        }

        static std::shared_ptr<chan<T>> select(chan<T>* ch1, chan<T>* ch2) {
            auto result = std::make_shared<chan<T>>(1);
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
                    for (int i = 0; i < 2; i++) {
                        auto ch = vec[i];
                        if (closed[i]) {
                            continue;
                        }
                        if (ch->is_closed()) {
                            closed[i] = true;
                            continue;
                        }
                        if (ch->readable()) {
                            auto curr_value = ch->recv();
                            // std::cout << "readable value: " << curr_value.value() << std::endl;
                            if (!curr_value.has_value()) {
                                throw logic_error("current channel is not readable");
                            }
                            result->send(curr_value.value());
                        }
                    }
                    // all closed
                    if (closed[0] && closed[1]) {
                        // std::cout << "closed \n"; 
                        result->close();
                        return;
                    }
                    std::unique_lock<std::mutex> lk{*mu};
                    cond->wait(lk);
                }
            };
            pool_.submit_task(push_data);
            return result;
        }

        static optional<int> select() {
            return {};
        }

    private:
        bool readable() const noexcept {
            if (closed_) {
                return false;
            }
            if (sz_ == 0) {
                // return !blocking_q_.empty();
            }
            return !q_.empty();
        }

    private:
        void register_send_callback(std::function<void()> send_callback) {
            send_callback_ = send_callback;
        }

    private:
        blocking_queue<T> q_;
        blocking_queue<bool> blocking_q_;
        const size_t sz_;

        std::atomic<bool> finished_send_;
        std::atomic<bool> closed_;
        std::function<void()> send_callback_ = nullptr;
        std::shared_ptr<std::condition_variable> close_cond_ = nullptr;

        static std::thread t_;
        static BS::thread_pool pool_;
    };

    template <typename T>
    BS::thread_pool chan<T>::pool_{4};
}  // namespace wwc