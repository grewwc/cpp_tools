#include <condition_variable>
#include <mutex>

namespace wwc {

    class semaphore {
    public:
        explicit semaphore(int n = 0) : count_{0}, cap_{n} {
        }

    public:
        void release() {
            std::lock_guard<std::mutex> lock(mutex_);
            ++count_;
            condition_.notify_one();
        }

        void acquire() {
            std::unique_lock<std::mutex> lock(mutex_);
            while (count_ <= 0)
                condition_.wait(lock);
            --count_;
        }

        bool try_acquire() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (count_ > 0) {
                --count_;
                return true;
            }
            return false;
        }

    private:
        std::mutex mutex_;
        std::condition_variable condition_;
        int count_ = 0;
        const int cap_;
    };
}  // namespace wwc