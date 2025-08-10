#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>

template<typename T>
class SPSCQueueLocked {
public:
    explicit SPSCQueueLocked(size_t capacity)
        : buffer(capacity + 1), head_(0), tail_(0) {}


    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return !full(); });

        buffer[head_] = value;
        head_ = (head_ + 1) % buffer.size();

        lock.unlock();
        not_empty.notify_one();
    }

    void pop(T& out) {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return !empty(); });

        out = buffer[tail_];
        tail_ = (tail_ + 1) % buffer.size();
        
        lock.unlock();
        not_full.notify_one();
    }

private:
    bool empty() const { return head_ == tail_; }
    bool full() const { return (head_ + 1) % buffer.size() == tail_; }

        

    std::vector<T> buffer;
    std::mutex mtx;
    size_t head_;
    size_t tail_;

    std::condition_variable not_full;
    std::condition_variable not_empty;

};

