#pragma once
#include <array>
#include <cstddef>
#include <utility>
#include <type_traits>

template <typename T, std::size_t N>
class RingBuffer {
    static_assert(N >= 2, "RingBuffer storage must be at least 2.");
public:
    using value_type = T;
    using size_type  = std::size_t;

    // zero-init indices; buffer default-initialized
    constexpr RingBuffer() noexcept = default;

    // non-copyable if T is non-copyable; default the big five when possible
    constexpr RingBuffer(const RingBuffer&) = default;
    constexpr RingBuffer& operator=(const RingBuffer&) = default;
    constexpr RingBuffer(RingBuffer&&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;
    constexpr RingBuffer& operator=(RingBuffer&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;

    // number of elements currently stored
    constexpr size_type size() const noexcept {
        return (head_ + N - tail_) % N;
    }

    // usable capacity (because we leave one slot)
    static constexpr size_type capacity() noexcept { return N - 1; }

    constexpr bool empty() const noexcept { return head_ == tail_; }
    constexpr bool full()  const noexcept { return next_(head_) == tail_; }

    // push by copy
    bool push(const T& v) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        auto next = next_(head_);
        if (next == tail_) return false; // full
        buf_[head_] = v;
        head_ = next;
        return true;
    }

    // push by move
    bool push(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>) {
        auto next = next_(head_);
        if (next == tail_) return false; // full
        buf_[head_] = std::move(v);
        head_ = next;
        return true;
    }

    // perfect-forwarding emplace
    template <class... Args>
    bool emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        auto next = next_(head_);
        if (next == tail_) return false; // full
        buf_[head_] = T(std::forward<Args>(args)...);
        head_ = next;
        return true;
    }

    // pop into out-parameter
    bool pop(T& out) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        if (empty()) return false;
        out   = buf_[tail_];
        tail_ = next_(tail_);
        return true;
    }

    // try_pop returning a value (optional convenience; same cost)
    bool try_pop(T& out) noexcept(std::is_nothrow_move_assignable_v<T>) {
        if (empty()) return false;
        out   = std::move(buf_[tail_]);
        tail_ = next_(tail_);
        return true;
    }

private:
    static constexpr size_type next_(size_type i) noexcept {
        if constexpr ((N & (N - 1)) == 0) {
            // power-of-two storage: cheap wrap
            return (i + 1) & (N - 1);
        } else {
            return (i + 1) % N;
        }
    }

    std::array<T, N> buf_{};
    size_type head_ = 0;
    size_type tail_ = 0;
};

