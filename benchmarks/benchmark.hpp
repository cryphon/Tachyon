#pragma once
#include <chrono>
#include <iostream>
#include <string>

namespace bench {
template<typename Func>
void run(const std::string&name, Func&& func, size_t ops) {
    /* Before measuring, we do a warm up loop to:
        *   - Get CPU branch predictors warmed up
        *   - Get buffer into L1 cache
        *   - Avoid measuring startup quirks
        *  Skipping this can lead to misleadingly slow results 
        */    
    func();

    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double seconds = ns / 1e9;
    double ops_per_sec = ops / seconds;
    double ns_per_op = double(ns) / double(ops);


    std::cout << name << "\n";
    std::cout << "  Total time: " << seconds << "s\n";
    std::cout << "  Ops/sec: " << ops_per_sec << "\n";
    std::cout << "  ns/op: " << ns_per_op << "\n\n";
}
}
