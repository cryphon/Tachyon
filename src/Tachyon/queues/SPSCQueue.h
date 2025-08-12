#pragma once
#include <atomic>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

namespace Tachyon::queues {

template <class T>
class SPSCQueue {
public:
    explicit SPSCQueue(size_t capacity)
        : cap_(capacity + 1),
        is_pot_((cap_ & (cap_ - 1)) == 0),
        mask_(is_pot_ ? (cap_ - 1) : 0),
        storage_(cap_) {
            head_.store(0, std::memory_order_relaxed);
            tail_.store(0, std::memory_order_relaxed);
    }

    size_t capacity() const noexcept { return cap_ - 1; }

    bool try_push(const T& v) { return do_push(v); }
    bool try_push(T&& v) { return do_push(std::move(v)); }

    template<class... Args>
    bool emplace(Args&&... args) {
        size_t h = head_.load(std::memory_order_relaxed);
        size_t t = tail_.load(std::memory_order_acquire);
        size_t next = next_(h);
        if(next == t) return false; // full
        storage_[h] = T(std::forward<Args>(args)...);
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool try_pop(T& out) {
        size_t t = tail_.load(std::memory_order_relaxed);
        size_t h = head_.load(std::memory_order_acquire);
        if (t == h) return false; // empty
        out = std::move(storage_[t]);
        tail_.store(next_(t), std::memory_order_release);
        return true;
    }

    bool empty() const noexcept {
        return tail_.load(std::memory_order_acquire) ==
               head_.load(std::memory_order_acquire);
    }

    bool full() const noexcept {
        size_t h = head_.load(std::memory_order_acquire);
        size_t t = tail_.load(std::memory_order_acquire);
        return next_(h) == t;
    }

private:
    template<class U>
    bool do_push(U&& v) {
        size_t h = head_.load(std::memory_order_relaxed);
        size_t t = tail_.load(std::memory_order_acquire);
        size_t next = next_(h);
        if (next == t) return false; // full
        storage_[h] = std::forward<U>(v);
        head_.store(next, std::memory_order_release);
        return true;
    }

    size_t next_(size_t i) const noexcept {
        size_t j = i + 1;
        return is_pot_ ? (j & mask_) : (j == cap_ ? 0 : j);
    }


    size_t cap_, mask_;
    bool is_pot_;
    std::vector<T> storage_;    // OK for SPSC; produces/consumer touch disjoint indices
    std::atomic<size_t> head_, tail_;



};
} // namespace Tachyon::queues
