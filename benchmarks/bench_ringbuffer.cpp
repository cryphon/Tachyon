#include "RingBuffer.h"
#include "benchmark.hpp"

int main() {
    constexpr size_t N = 10'000'000;
    RingBuffer<int, 1024> rb;
    volatile int sink = 0;

    bench::run("Single-thread RingBuffer", [&] {
        for (size_t i = 0; i < N; ++i) {
            rb.push(static_cast<int>(i));
            int out;
            rb.pop(out);

            // avoid auto compiler optimizations
            sink += out;
        }
    }, N * 2); // 2 ops per iteration
}

