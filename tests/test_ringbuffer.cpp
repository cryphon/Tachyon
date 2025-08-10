#include "RingBuffer.h"
#include <cassert>
#include <cstdio>
#include <string>

// test macro
#define CHECK(expr) do { if(!(expr)) { \
    std::fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
    return 1; } } while(0)

template<std::size_t N>
int test_int_buffer_basic() {
    RingBuffer<int, N> rb;

    // 1) empty on init
    CHECK(rb.empty());
    CHECK(!rb.full());
    CHECK(rb.size() == 0);

    // special case: N == 1 would violate our static_assert; we require N >= 2
    static_assert(N >= 2);

    // 2) push until full (usable slots = N-1)
    for(std::size_t i = 0; i < N - 1; ++i) {
        CHECK(rb.push(static_cast<int>(i)));
        CHECK(rb.size() == i + 1);
    }

    CHECK(rb.full());
    CHECK(!rb.push(999)); // overflow should fail

    // 3) pop a few to force wrap behavior on subsequent pushes
    int out = -1;
    for (std::size_t i = 0; i < (N - 1) / 2; ++i) {
        CHECK(rb.pop(out));
        CHECK(out == static_cast<int>(i));
    }

    // 4) push again to wrap head over index 0
    for (std::size_t i = 0; i < (N - 1) / 2; ++i) {
        CHECK(rb.push(1000 + static_cast<int>(i)));
    }

    // 5) pop everything and verify FIFO order across wrap
    std::size_t expected = (N - 1) / 2;
    std::size_t off = 0;
    while (!rb.empty()) {
        CHECK(rb.pop(out));
        if (expected < (N - 1)) {
            CHECK(out == static_cast<int>(expected));
            ++expected;
        } else {
            // wrapped values
            CHECK(out == 1000 + static_cast<int>(off));
            ++off;
        }
    }
    CHECK(rb.size() == 0);
    CHECK(!rb.pop(out)); // underflow should fail
    return 0;
}

int test_string_buffer() {
    // Simple sanity for non-POD type
    RingBuffer<std::string, 4> rb;
    CHECK(rb.empty());
    CHECK(rb.push(std::string("a")));
    CHECK(rb.push(std::string("b")));
    std::string s;
    CHECK(rb.pop(s) && s == "a");
    CHECK(rb.pop(s) && s == "b");
    return 0;
}

int main(void) {
    CHECK(test_int_buffer_basic<2>() == 0);
    CHECK(test_int_buffer_basic<3>() == 0);
    CHECK(test_int_buffer_basic<4>() == 0);
    CHECK(test_int_buffer_basic<8>() == 0);
    CHECK(test_string_buffer() == 0);
    return 0;
}
