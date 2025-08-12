// bench_ringbuffer.cpp
#include <Tachyon/ring/RingBufferFixed.h>
#include "benchmark.hpp"
#include <cstddef>

using Tachyon::ring::RingBufferFixed;

int main() {
    bench::run_queue_benchmark<RingBufferFixed<int>>(
        "RingBufferFixed<int>", bench::Mode::SingleThread, 10'000'000
    );
}



