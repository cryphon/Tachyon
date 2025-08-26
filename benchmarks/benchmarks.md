# Benchmark statistics

All tests are compiled with `-O3 -march=native -DnDEBUG`. CPU pinning is done using `taskset -c 3` unless otherwise specified. Results represent **single runs**; absolute values may vary slightly depending on system load, but relative trends are stable.

---

## Classic RingBuffer

Fixed-size ring buffer with power-of-two indexing, benchmarked in a single thread.

```bash
ST RingBuffer (N=1024, int)
  Total time: 0.00648115s
  Ops/sec: 3.08587e+09
  ns/op: 0.324057
```

**Interpretation:** This is close to peak raw memory throughput: \~0.3 ns per operation is just a handful of CPU cycles. This serves as a baseline for the *fastest possible circular buffer* implementation.

---

## RingBufferQueue (without POT optimizations)

Indexing without power-of-two masking.

```bash
Single-thread RingBufferQueue NO-POT
  Total time: 0.0841611s
  Ops/sec: 2.37639e+08
  ns/op: 4.20806
```

**Interpretation:** Roughly an order of magnitude slower than the classic ring buffer. The cost comes from modulo arithmetic (`%`) rather than bitmasking.

---

## RingBufferQueue (with POT optimizations)

Indexing optimized with power-of-two masking.

```bash
Single-thread RingBufferQueue POT
  Total time: 0.098163s
  Ops/sec: 2.03743e+08
  ns/op: 4.90815
```

**Interpretation:** Somewhat unexpectedly, this run shows **slightly worse performance** than the NO-POT variant. This suggests that the implementation may have other inefficiencies or the test setup introduces variance. In principle, POT should outperform NO-POT.

---

## SPSCQueue

Single-producer/single-consumer lock-free queue with separated head/tail indices. Benchmarks include both a synthetic single-thread run (baseline) and a real producer/consumer test across threads.

`./bench_spscqueue_st`

```bash
SPSCQueue<int> [SingleThread]
  Total time: 0.0147115s
  Ops/sec: 1.35948e+09
  ns/op: 0.735575
```

`./bench_spscqueue_spsc`

```bash
SPSCQueue<int> [SPSC]
  Total time: 0.732595s
  Ops/sec: 2.73002e+07
  ns/op: 36.6298
```

**Interpretation:**

* In the synthetic single-thread mode, throughput is only \~2× slower than the raw ring buffer, showing minimal abstraction overhead.
* In the real SPSC case, performance drops to \~27M ops/sec (≈37 ns/op). This is consistent with the expected cost of cross-core cache line transfers and memory fences in a lock-free queue. These results are in line with known performance of production-grade SPSC queues (e.g., Folly, moodycamel).

---



