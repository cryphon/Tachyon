#include "RingBufferQueue.h"
#include "benchmark.hpp"



int main() {
    constexpr size_t N = 10'000'000;
    RingBufferQueue<int> spsc(1024);
    volatile int sink = 0;

    bench::run("Single-thread SPSC Queue Locked RingBuffer", [&] {
        for (size_t i = 0; i < N; ++i) {
            spsc.push(static_cast<int>(i));
            int out;
            spsc.pop(out);

            // avoid auto compiler optimizations for out deletion
            sink += out;
        }
    }, N * 2); // 2 ops per iteration
}

