#include <condition_variable>
#include <mutex>

namespace wwc {

    class semaphore {
        std::mutex mutex_;
        std::condition_variable condition_;
        long count_ = 0;

    public:
        void release() {
            std::lock_guard<decltype(mutex_)> lock(mutex_);
            ++count_;
            condition_.notify_one();
        }

        void acquire() {
            std::unique_lock<decltype(mutex_)> lock(mutex_);
            while (count_ <= 0)  // Handle spurious wake-ups.
                condition_.wait(lock);
            --count_;
        }

        bool try_acquire() {
            std::lock_guard<decltype(mutex_)> lock(mutex_);
            if (count_) {
                --count_;
                return true;
            }
            return false;
        }
    };
}  // namespace wwc