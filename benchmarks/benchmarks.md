# Benchmark statistics

All tests are ran using `-O3 -march=native -DnDEBUG` as of now


### classic RingBuffer
`taskset -c 3 ./bench_ringbuffer`
```bash
ST RingBuffer (N=1024, int)
  Total time: 0.00648115s
  Ops/sec: 3.08587e+09
  ns/op: 0.324057
```

### RingBufferQueue without POT(Power Of Two) optimizations for idx
`taskset -c 3 ./bench_ringbufferqueue`
```bash
Single-thread RingBufferQueue NO-POT
  Total time: 0.0841611s
  Ops/sec: 2.37639e+08
  ns/op: 4.20806
```
### RingBufferQueue with POT optimizations
It seems there might have been an issue with the NO-POT implementation. Based on this run we can determine with the correct operations that this implementation is slower(?)
`taskset -c 3 ./bench_ringbufferqueue`
```bash
Single-thread RingBufferQueue POT
    Total time: 0.098163s 
    Ops/sec: 2.03743e+08 
    ns/op: 4.90815
```
