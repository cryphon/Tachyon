#include "RingBufferQueue.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <cstddef>

// test macro
#define CHECK(expr) do { if(!(expr)) { \
    std::fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
    return 1; } } while(0)

// NOTE: Unlike RingBuffer, RingBufferQueue uses blocking push/pop and doesn't
// expose empty/full/size. These tests avoid calling push on a full queue or
// pop on an empty queue to prevent blocking.

// Basic FIFO & wrap-around behavior for integers
// CAP is the *usable* capacity requested in the constructor

template<std::size_t CAP>
int test_int_queue_basic() {
    static_assert(CAP >= 1, "RingBufferQueue supports CAP >= 1");
    RingBufferQueue<int> q(CAP);

    // 1) Push exactly CAP elements (queue becomes full but does not block)
    for (std::size_t i = 0; i < CAP; ++i) {
        q.push(static_cast<int>(i));
    }

    // 2) Pop half of them to create space and move tail (exercise partial drain)
    const std::size_t half = CAP / 2; // truncates toward 0; fine for CAP==1 (half=0)
    int out = -1;
    for (std::size_t i = 0; i < half; ++i) {
        q.pop(out);
        CHECK(out == static_cast<int>(i));
    }

    // 3) Push another `half` elements to force head wrap-around
    for (std::size_t i = 0; i < half; ++i) {
        q.push(1000 + static_cast<int>(i));
    }

    // 4) Pop everything and verify FIFO order across wrap
    std::size_t expected = half;        // continue from where we left off
    std::size_t wrapped_index = 0;      // counts how many 1000+ values we've seen

    // Remaining items should be (CAP - half) original values, then `half` wrapped values
    for (std::size_t k = 0; k < (CAP - half) + half; ++k) {
        q.pop(out);
        if (expected < CAP) {
            CHECK(out == static_cast<int>(expected));
            ++expected;
        } else {
            CHECK(out == 1000 + static_cast<int>(wrapped_index));
            ++wrapped_index;
        }
    }

    return 0;
}

// Simple sanity for non-POD types
int test_string_queue() {
    RingBufferQueue<std::string> q(4);
    q.push(std::string("a"));
    q.push(std::string("b"));
    std::string s;
    q.pop(s); CHECK(s == "a");
    q.pop(s); CHECK(s == "b");
    return 0;
}

int main() {
    CHECK(test_int_queue_basic<1>() == 0);
    CHECK(test_int_queue_basic<2>() == 0);
    CHECK(test_int_queue_basic<4>() == 0);
    CHECK(test_int_queue_basic<8>() == 0);
    CHECK(test_string_queue() == 0);
    return 0;
}

