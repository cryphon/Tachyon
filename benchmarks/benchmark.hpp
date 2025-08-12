#pragma once
#include <chrono>
#include <iostream>
#include <string>

#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <cassert>

namespace bench {

enum class Mode { SingleThread, SPSC };

struct Result {
    double seconds;
    double ops_per_sec;
    double ns_per_op;
};

inline void print_result(const std::string& name, const Result& r) {
    std::cout << name << "\n";
    std::cout << "  Total time: " << r.seconds << "s\n";
    std::cout << "  Ops/sec: " << r.ops_per_sec << "\n";
    std::cout << "  ns/op: " << r.ns_per_op << "\n\n";
}

template<typename SetupFunc, typename RunFunc>
Result run_once(SetupFunc&& setup, RunFunc&& run, size_t ops) {
    setup();
    auto start = std::chrono::high_resolution_clock::now();
    run();
    auto end = std::chrono::high_resolution_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double seconds = ns / 1e9;
    double ops_per_sec = ops / seconds;
    double ns_per_op = double(ns) / double(ops);
    return {seconds, ops_per_sec, ns_per_op};
}

// Benchmark a queue-like type in different modes
template<typename Queue>
void run_queue_benchmark(const std::string& name, Mode mode, size_t iterations) {
    if (mode == Mode::SingleThread) {
        Queue q(1024);
        // Warm-up and timing
        auto res = run_once(
            [&] { new(&q) Queue(1024); }, // setup
            [&] {
                int out;
                for (size_t i = 0; i < iterations; ++i) {
                    while (!q.try_push(static_cast<int>(i))) {}
                    while (!q.try_pop(out)) {}
                }
            },
            iterations * 2 // push + pop per iteration
        );
        print_result(name + " [SingleThread]", res);
    }
    else if (mode == Mode::SPSC) {
        Queue q(1024);
        std::atomic<bool> start_flag{false};
        std::atomic<size_t> produced{0}, consumed{0};

        auto res = run_once(
            [&] { new(&q) Queue(1024); }, // setup
            [&] {
                std::thread prod([&] {
                    while (!start_flag.load(std::memory_order_acquire)) {}
                    for (size_t i = 0; i < iterations; ++i) {
                        while (!q.try_push(static_cast<int>(i))) {}
                        produced.fetch_add(1, std::memory_order_relaxed);
                    }
                });

                std::thread cons([&] {
                    int out;
                    while (!start_flag.load(std::memory_order_acquire)) {}
                    for (size_t i = 0; i < iterations; ++i) {
                        while (!q.try_pop(out)) {}
                        consumed.fetch_add(1, std::memory_order_relaxed);
                    }
                });

                start_flag.store(true, std::memory_order_release);
                prod.join();
                cons.join();
            },
            iterations * 2 // push + pop per iteration
        );
        print_result(name + " [SPSC]", res);
        assert(produced == iterations && consumed == iterations && "Mismatch in produced/consumed counts!");
    }
}

} // namespace bench

