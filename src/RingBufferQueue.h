#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>

template<typename T>
class RingBufferQueue {
public:
    explicit RingBufferQueue(size_t capacity)
        : buffer(capacity + 1),
        head_(0),
        tail_(0),
        cap_(capacity + 1),                              // <-- actual ring size
        is_pot_((cap_ & (cap_ - 1)) == 0),              // <-- based on cap_
        mask_(is_pot_ ? (cap_ - 1) : 0)                 // <-- only valid if POT
    {}

    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return !full(); });

        buffer[head_] = value;
        head_ = next_(head_);

        lock.unlock();
        not_empty.notify_one();
    }

    void pop(T& out) {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return !empty(); });

        out = buffer[tail_];
        tail_ = next_(tail_);
        
        lock.unlock();
        not_full.notify_one();
    }

private:
    bool empty() const { return head_ == tail_; }
    bool full() const { return (head_ + 1) % buffer.size() == tail_; }
    inline size_t next_(size_t i) const noexcept {
        size_t j = i + 1;
        if (is_pot_) {
            return j & mask_;
        } else {
            // cheap wrap since we only add 1
            return (j == cap_) ? 0 : j;
        }
    };

    size_t cap_;
    bool is_pot_;
    size_t mask_; // cap -1 when is_pot_ == true
        

    std::vector<T> buffer;
    std::mutex mtx;
    size_t head_;
    size_t tail_;

    std::condition_variable not_full;
    std::condition_variable not_empty;

};

