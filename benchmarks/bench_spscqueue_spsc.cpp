#include <Tachyon/queues/SPSCQueue.h>
#include "benchmark.hpp"
using Tachyon::queues::SPSCQueue;

int main() {
    bench::run_queue_benchmark<SPSCQueue<int>>(
        "SPSCQueue<int>", bench::Mode::SPSC, 10'000'000
    );
}

