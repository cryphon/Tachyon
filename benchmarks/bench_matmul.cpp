#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <Tachyon/linalg/MatMul.h>

using clk = std::chrono::high_resolution_clock;

template<typename Fn>
double time_ms(Fn&& f, int iters=1) {
    auto t0 = clk::now();
    for (int i=0;i<iters;++i) f();
    auto t1 = clk::now();
    std::chrono::duration<double, std::milli> d = t1 - t0;
    return d.count() / iters;
}

double gflops(std::size_t M, std::size_t N, std::size_t K, double ms) {
    const double ops = 2.0 * double(M) * double(N) * double(K);
    return (ops / 1e9) / (ms / 1e3);
}

template<typename T>
bool nearly_equal(const std::vector<T>& X, const std::vector<T>& Y, double tol=1e-6) {
    if (X.size() != Y.size()) return false;
    for (std::size_t i=0;i<X.size();++i)
        if (std::abs(double(X[i]) - double(Y[i])) > tol) return false;
    return true;
}

struct Variant {
    std::string name;
    std::function<void(const double*, const double*, double*, std::size_t, std::size_t, std::size_t)> fn;
};

int main(int argc, char** argv) {
    // choose sizes from CLI (e.g., `./bench_matmul 256 512 1024`), or default sweep
    std::vector<std::size_t> Ns;
    for (int i=1;i<argc;++i) Ns.push_back(std::stoul(argv[i]));
    if (Ns.empty()) Ns = {128, 256, 384, 512, 768, 1024};

    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    std::vector<Variant> variants = {
        {"ijk",  tachyon::linalg::mm_ijk<double>},
        {"ikj",  tachyon::linalg::mm_ikj<double>},
    };

    // formatting helpers
    auto hr = []{ std::cout << std::string(66, '-') << "\n"; };

    for (auto N : Ns) {
        const std::size_t M=N, K=N;
        std::vector<double> A(M*K), B(K*N), C(M*N), Ref(M*N), BT(N*K);

        for (auto& a : A) a = dist(rng);
        for (auto& b : B) b = dist(rng);

        // reference & warm
        tachyon::linalg::mm_ikj(A.data(), B.data(), Ref.data(), M, N, K);
        for (int w=0; w<2; ++w) tachyon::linalg::mm_ikj(A.data(), B.data(), C.data(), M, N, K);

        std::cout << "\nSize N=" << N << "  (A:" << M << "x" << K
                  << ", B:" << K << "x" << N << ", C:" << M << "x" << N << ")\n";
        hr();
        std::cout << std::left << std::setw(20) << "Variant"
                  << std::right << std::setw(12) << "Time (ms)"
                  << std::setw(14) << "GFLOP/s" << "\n";
        hr();

        // measure 6 permutations
        for (const auto& v : variants) {
            // correctness
            v.fn(A.data(), B.data(), C.data(), M, N, K);
            if (!nearly_equal(C, Ref, 1e-6)) {
                std::cerr << "[ERROR] " << v.name << " != reference\n";
                return 1;
            }

            // timing = median of 5
            std::vector<double> samples;
            for (int r=0;r<5;++r)
                samples.push_back(time_ms([&]{ v.fn(A.data(), B.data(), C.data(), M, N, K); }));
            std::sort(samples.begin(), samples.end());
            const double ms = samples[samples.size()/2];
            const double gf = gflops(M,N,K,ms);

            std::cout << std::left << std::setw(20) << v.name
                      << std::right << std::setw(12) << std::fixed << std::setprecision(3) << ms
                      << std::setw(14) << std::setprecision(2) << gf << "\n";
        }
    }
    return 0;
}

