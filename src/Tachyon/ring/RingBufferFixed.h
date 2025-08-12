#pragma once
#include <vector>
#include <type_traits>
#include <utility>

namespace Tachyon::ring {

template <class T>
class RingBufferFixed {
public:
    explicit RingBufferFixed(size_t capacity)
        : cap_(capacity),             
        is_pot_((cap_ & (cap_ - 1)) == 0),
        mask_(is_pot_ ? (cap_ - 1) : 0),
        buf_(cap_), head_(0), tail_(0) {}

    size_t capacity() const noexcept { return cap_ - 1; }
    bool empty()   const noexcept { return head_ == tail_; }
    bool full()    const noexcept { return next_(head_) == tail_; }

    bool try_push(const T& v) {
        if (full()) return false;
        buf_[head_] = v;
        head_ = next_(head_);
        return true;
    }
    bool try_push(T&& v) {
        if (full()) return false;
        buf_[head_] = std::move(v);
        head_ = next_(head_);
        return true;
    }
    template<class... Args>
    bool emplace(Args&&... args) {
        if (full()) return false;
        buf_[head_] = T(std::forward<Args>(args)...);
        head_ = next_(head_);
        return true;
    }

    bool try_pop(T& out) {
        if (empty()) return false;
        out = std::move(buf_[tail_]);
        tail_ = next_(tail_);
        return true;
    }

    const T* peek() const noexcept {
        return empty() ? nullptr : &buf_[tail_];
    }

    void clear() noexcept { head_ = tail_ = 0; }

    size_t size() const noexcept {
        return (head_ + cap_ - tail_) % cap_;
    }

private:
    size_t next_(size_t i) const noexcept {
        size_t j = i + 1;
        return is_pot_ ? (j & mask_) : (j == cap_ ? 0 : j);
    }

    size_t cap_;
    bool   is_pot_;
    size_t mask_;
    std::vector<T> buf_;
    size_t head_, tail_;
};

} // namespace Tachyon::ring

