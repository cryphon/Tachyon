// bench_ringbuffer.cpp
#include <Tachyon/ring/RingBufferFixed.h>
#include "benchmark.hpp"
#include <cstddef>

using Tachyon::ring::RingBufferFixed;

int main() {
    constexpr std::size_t N = 10'000'000;
    RingBufferFixed<int> rb(1024);
    volatile int sink = 0;

    bench::run("Single-thread RingBuffer try_push/try_pop", [&] {
        // Keep the buffer at size 0 or 1 so try_* never blocks/allocates
        for (std::size_t i = 0; i < N; ++i) {
            bool ok1 = rb.try_push(static_cast<int>(i));
            // Should always succeed because we pop immediately
            if (!ok1) __builtin_trap();

            int out;
            bool ok2 = rb.try_pop(out);
            if (!ok2) __builtin_trap();

            sink += out; // defeat some optimizations
        }
    }, N * 2); // 2 ops per iteration
}


