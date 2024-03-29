#include "hierachy_mutex.hpp"

#include <exception>
#include <limits>
#include <sstream>

namespace wwc {
    thread_local int hierachy_mutex::curr_thread_value_ = std::numeric_limits<int>::max();

    bool hierachy_mutex::check() {
        return curr_thread_value_ >= value_;
    }

    void hierachy_mutex::lock() {
        if (!check()) {
            std::stringstream ss;
            ss << "hieracy(" << curr_thread_value_ << ") is locked, unable to lock(" << value_ << ")";
            throw std::logic_error(ss.str());
        }
        mu_.lock();
        prev_value_ = curr_thread_value_;
        curr_thread_value_ = value_;
    }

    void hierachy_mutex::unlock() {
        if (curr_thread_value_ != value_) {
            std::stringstream ss;
            ss << "curr_thread_value != value (" << curr_thread_value_ << "!=" << value_;
            throw std::logic_error(ss.str());
        }
        curr_thread_value_ = prev_value_;
        mu_.unlock();
    }

    bool hierachy_mutex::try_lock() {
        if (!check()) {
            return false;
        }
        if (!mu_.try_lock()) {
            return false;
        }
        prev_value_ = curr_thread_value_;
        curr_thread_value_ = value_;
        return true;
    }
}  // namespace wwc