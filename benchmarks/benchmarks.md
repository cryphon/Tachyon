# Benchmark statistics

All tests are ran using `-O3 -march=native -DnDEBUG` as of now


### classic RingBuffer
```bash
ST RingBuffer (N=1024, int)
  Total time: 0.00648115s
  Ops/sec: 3.08587e+09
  ns/op: 0.324057
```

### SPSC Queue Locked RingBuffer without POT(Power Of Two) optimizations for idx
```bash
Single-thread SPSC Queue Locked RingBuffer
  Total time: 0.0841611s
  Ops/sec: 2.37639e+08
  ns/op: 4.20806
```
