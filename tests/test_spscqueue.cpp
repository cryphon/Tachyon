#include <Tachyon/queues/SPSCQueue.h>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>
#include <cstdio>

using Tachyon::queues::SPSCQueue;

// simple spin backoff helper
static inline void cpu_relax() {
#if defined(__x86_64__) || defined(_M_X64)
    asm volatile("pause" ::: "memory");
#endif
}

#define CHECK(expr) do { if(!(expr)) { \
  std::fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
  return 1; } } while(0)

int main() {
    const std::size_t N = 1'000'00; // 100k
    SPSCQueue<int> q(1024);

    std::atomic<bool> started{false};
    std::vector<int> consumed;
    consumed.reserve(N);

    std::thread producer([&]{
        while (!started.load(std::memory_order_acquire)) { cpu_relax(); }
        for (std::size_t i = 0; i < N; ++i) {
            while (!q.try_push(static_cast<int>(i))) { cpu_relax(); }
        }
    });

    std::thread consumer([&]{
        started.store(true, std::memory_order_release);
        int v;
        for (std::size_t i = 0; i < N; ++i) {
            while (!q.try_pop(v)) { cpu_relax(); }
            consumed.push_back(v);
        }
    });

    producer.join();
    consumer.join();

    CHECK(consumed.size() == N);
    for (std::size_t i = 0; i < N; ++i) {
        CHECK(consumed[i] == static_cast<int>(i));
    }
    return 0;
}

