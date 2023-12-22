#pragma once
#include <mutex>

class hierachy_mutex {
public:
    hierachy_mutex(int hierachy_value) : value_{hierachy_value} {}
    
    // lock
    void lock();
    // unlock
    void unlock();
    // try_lock
    bool try_lock();

  private:
    bool check();

private:
    const int value_;
    int prev_value_ = 0;
    static thread_local int curr_thread_value_;
    std::mutex mu_;
};