#include <Tachyon/ring/RingBufferFixed.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

using Tachyon::ring::RingBufferFixed;

// test macro
#define CHECK(expr) do { if(!(expr)) { \
  std::fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
  return 1; } } while(0)

int test_int_buffer_basic(std::size_t cap) {
    RingBufferFixed<int> rb(cap);  // runtime capacity (usable = cap - 1)
    CHECK(rb.empty());
    CHECK(!rb.full());
    CHECK(rb.size() == 0);
    CHECK(rb.capacity() == cap - 1);

    // 1) fill to full (N-1 usable)
    for (std::size_t i = 0; i < rb.capacity(); ++i) {
        CHECK(rb.try_push(static_cast<int>(i)));
        CHECK(rb.size() == i + 1);
    }
    CHECK(rb.full());
    CHECK(!rb.try_push(999));   // overflow should fail

    // 2) pop half to create space and force wrap later
    int out = -1;
    std::size_t half = rb.capacity() / 2;
    for (std::size_t i = 0; i < half; ++i) {
        CHECK(rb.try_pop(out));
        CHECK(out == static_cast<int>(i));
    }

    // 3) push again to wrap head
    for (std::size_t i = 0; i < half; ++i) {
        CHECK(rb.try_push(1000 + static_cast<int>(i)));
    }

    // 4) pop everything; verify FIFO across wrap
    std::size_t expected = half;
    std::size_t off = 0;
    while (!rb.empty()) {
        CHECK(rb.try_pop(out));
        if (expected < rb.capacity()) {
            CHECK(out == static_cast<int>(expected));
            ++expected;
        } else {
            CHECK(out == 1000 + static_cast<int>(off));
            ++off;
        }
    }
    CHECK(rb.size() == 0);
    int dummy;
    CHECK(!rb.try_pop(dummy)); // underflow should fail
    return 0;
}

int test_string_buffer() {
    RingBufferFixed<std::string> rb(4);
    CHECK(rb.empty());
    CHECK(rb.try_push(std::string("a")));
    CHECK(rb.try_push(std::string("b")));
    std::string s;
    CHECK(rb.try_pop(s) && s == "a");
    CHECK(rb.try_pop(s) && s == "b");
    return 0;
}

int main() {
    // Mix POT and non-POT to exercise mask and non-mask paths
    CHECK(test_int_buffer_basic(2) == 0);   // usable 1
    CHECK(test_int_buffer_basic(3) == 0);   // non-POT
    CHECK(test_int_buffer_basic(4) == 0);   // POT
    CHECK(test_int_buffer_basic(8) == 0);   // POT
    CHECK(test_int_buffer_basic(17) == 0);  // non-POT
    CHECK(test_string_buffer() == 0);
    return 0;
}

