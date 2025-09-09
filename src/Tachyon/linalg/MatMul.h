#pragma once
#include <cstddef>
#include <algorithm>
#include <cstring>

// Row-major matrices:
// A: MxK, B: KxN, C: MxN
// index(row, col, stride) = row*stride + col

#if defined(_MSC_VER)
    #define TACHYON_RESTRICT __restrict
#else
    #define TACHYON_RESTRICT __restrict__
#endif

namespace tachyon::linalg {

// --- utils ---
template<typename T>
inline void zero(T* C, size_t M, size_t N) noexcept {
    std::fill(C, C+ (M*N), T{0});
}

template<typename T>
inline void transpose(const T* B, T* BT, size_t rows, size_t cols) noexcept {
    // B: rows x cols -> BT: cols x rows
    for(size_t r = 0; r < rows; ++r) {
        for(size_t c = 0; c < cols; ++c) {
            BT[c * rows + r] = B[r * cols + c];
        }
    }
}

template <typename T>
inline void mm_ijk(const T* TACHYON_RESTRICT A, const T* TACHYON_RESTRICT B, T* TACHYON_RESTRICT C, size_t M, size_t N, size_t K) noexcept {
    zero(C, M, N);
    for(size_t i = 0; i < M; ++i) {
        for(size_t j = 0; j < N; ++j) {
            T sum = 0;
            for(size_t k = 0; k < K; ++k) {
                sum += A[i*K + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

// ikj: good locality for B (row walk) and C (row)
template<typename T>
inline void mm_ikj(const T* TACHYON_RESTRICT A,const T* TACHYON_RESTRICT B, T* TACHYON_RESTRICT C, std::size_t M, std::size_t N, std::size_t K) noexcept {
    zero(C, M, N);
    for (size_t i=0;i<M;++i) {
        for (size_t k=0;k<K;++k) {
            T a = A[i*K + k];
            const size_t b_row = k*N;
            size_t c_row = i*N;
            for (size_t j=0;j<N;++j)
                C[c_row + j] += a * B[b_row + j];
        }
    }
}

template<typename T>
inline void mm_jik(const T* TACHYON_RESTRICT A,const T* TACHYON_RESTRICT B, T* TACHYON_RESTRICT C, std::size_t M, std::size_t N, std::size_t K) noexcept {
    zero(C, M, N);
    for(size_t j = 0; j < N; ++j) {
        for(size_t i = 0; i < M; ++i) {
            T sum = 0;
            for(size_t k = 0; k < K; ++k) {
                sum += A[i*K + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}
}

