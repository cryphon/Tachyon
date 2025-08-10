#include "RingBuffer.hpp"
#include "benchmark.hpp"

int main() {
    constexpr size_t N = 10'000'000;
    auto rb = make_ring_buffer<int, 1024>();
    volatile int sink = 0;

    bench::run("Single-thread RingBuffer", [&] {
        for (size_t i = 0; i < N; ++i) {
            ring_buffer_push(rb, static_cast<int>(i));
            int out;
            ring_buffer_pop(rb, out);

            // avoid auto compiler optimizations
            sink += out;
        }
    }, N * 2); // 2 ops per iteration
}

