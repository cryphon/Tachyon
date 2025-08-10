#include "RingBuffer.hpp"
#include <cassert>
#include <string>

// test macro
#define CHECK(expr) do { if(!(expr)) { \
    fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
    return 1; } } while(0)


template<size_t capacity>
int test_int_buffer_basic() {
    auto rb = make_ring_buffer<int, capacity>();

    // 1) empty on init
    CHECK(ring_buffer_is_empty(rb));
    CHECK(!ring_buffer_is_full(rb));
    CHECK(ring_buffer_size(rb) == 0);


    // special case: capacity == 1 -> always empty, cannot push
    if constexpr (capacity == 1) {
        CHECK(!ring_buffer_push(rb, 42));
        CHECK(ring_buffer_is_empty(rb));
        CHECK(ring_buffer_size(rb) == 0);
        return 0;
    }

    // 2) push until full (usable slots = capacity-1
    for(size_t i = 0; i < capacity - 1; ++i) {
        CHECK(ring_buffer_push(rb, static_cast<int>(i)));
        CHECK(ring_buffer_size(rb) == i + 1);
    }

    CHECK(ring_buffer_is_full(rb));
    CHECK(!ring_buffer_push(rb, 999)); // overflow should fail
    

    // 3) pop a few to force wrap behavior on subsequent pushes
    int out = -1;
    for (size_t i = 0; i < (capacity - 1) / 2; ++i) {
        CHECK(ring_buffer_pop(rb, out));
        CHECK(out == static_cast<int>(i));
    }


    // 4) push again to wrap head over index 0
    for (size_t i = 0; i < (capacity - 1) / 2; ++i) {
        CHECK(ring_buffer_push(rb, 1000 + static_cast<int>(i)));
    }

    // 5) pop everything and verify FIFO order across wrap
    size_t expected = (capacity - 1) / 2;
    size_t off = 0;
    while (!ring_buffer_is_empty(rb)) {
        CHECK(ring_buffer_pop(rb, out));
        if (expected < (capacity - 1)) {
            CHECK(out == static_cast<int>(expected));
            ++expected;
        } else {
            // wrapped values
            CHECK(out == 1000 + static_cast<int>(off));
            ++off;
        }
    }
    CHECK(ring_buffer_size(rb) == 0);
    CHECK(!ring_buffer_pop(rb, out)); // underflow should fail
    return 0;
}


int test_string_buffer() {
    // Simple sanity for non-POD type
    auto rb = make_ring_buffer<std::string, 4>();
    CHECK(ring_buffer_is_empty(rb));
    CHECK(ring_buffer_push(rb, std::string("a")));
    CHECK(ring_buffer_push(rb, std::string("b")));
    std::string s;
    CHECK(ring_buffer_pop(rb, s) && s == "a");
    CHECK(ring_buffer_pop(rb, s) && s == "b");
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
    
