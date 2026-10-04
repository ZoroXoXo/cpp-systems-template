# C++20 High-Performance Systems & Low-Latency Laboratory

A production-grade C++20 development infrastructure and micro-architectural profiling laboratory built for low-latency systems engineering, quantitative trading engines, and high-throughput networking.

---

## 🛠️ Toolchain & Development Environment
- **Operating System:** Arch Linux
- **Compiler:** Clang / LLVM (C++20)
- **Linker:** LLD (`-fuse-ld=lld`)
- **Build System:** CMake + Ninja
- **Microbenchmarking:** Google Benchmark
- **Hardware PMU Profiling:** Linux `perf`
- **Sanitizers:** AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan), ThreadSanitizer (TSan)

---

## 📊 Performance Benchmarks & Results

| Component / Experiment | Baseline Latency / Throughput | Optimized Latency / Throughput | Performance Speedup / Result |
| :--- | :--- | :--- | :--- |
| **std::vector vs. std::list Traversal** | ~87.6 µs (`std::list`) | **~4.5 µs** (`std::vector`) | **~19.5x faster** (Contiguous L1/L2 prefetching vs. heap pointer chasing) |
| **False Sharing (MESI Protocol)** | ~15.6 ms (Unpadded) | **~5.2 ms** (`alignas(64)`) | **~3.0x speedup** (Eliminated cross-core cache invalidation bouncing) |
| **Lock-Free SPSC Queue** | Standard Mutex / Locks | **~4.64 ns / msg** (~215M msgs/sec) | Sub-5ns lock-free inter-thread messaging with acquire-release semantics |
| **Custom Fixed-Block Allocator** | ~16.28 µs (`malloc` / `free`) | **~4.63 µs** ($O(1)$ Free-List Pool) | **~3.51x faster** (Eliminated system heap allocation non-determinism) |

---

## 🔬 Core Components & Architecture

### 1. Lock-Free SPSC Ring Buffer Queue (`include/spsc_queue.hpp`)
- **Memory Ordering:** Atomic acquire-release semantics (`std::memory_order_acquire` / `release`) forming a strict happens-before relationship between producer and consumer.
- **Cache Alignment:** `alignas(64)` padding on `head_` and `tail_` pointers to eliminate false sharing.
- **Bitwise Indexing:** Power-of-two ring buffer capacity utilizing bitwise AND masking (`index & (Capacity - 1)`).
- **Index Caching:** Local shadow copies of `head_` and `tail_` to prevent cross-core interconnect traffic on every push/pop.
- **Validation:** Verified 100% thread-safe under multi-threaded stress with **ThreadSanitizer (TSan)**.

### 2. Fixed-Size Block Memory Pool Allocator (`include/fixed_block_allocator.hpp`)
- **Deterministic Latency:** $O(1)$ allocation and deallocation times using an in-place singly-linked free list.
- **Zero Dynamic Allocation:** Pre-allocates contiguous memory blocks up front, avoiding system calls (`brk`/`mmap`) in core hot paths.
- **Constructors:** In-place object construction using placement `new` and C++20 variadic perfect forwarding.

### 3. Limit Order Book Engine (`include/limit_order_book.hpp`)
- **Price-Time Priority:** Doubly-linked price level lists maintaining FIFO execution order.
- **Pooled Storage:** Plugs in `FixedBlockAllocator` for zero-allocation order creation and cancellations.

---

## ⚡ Build & Benchmark Instructions

### Release Build (Optimized `-O3 -march=native`)
```bash
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build-release

# Run Microbenchmarks
./build-release/bench_spsc
./build-release/bench_allocator
./build-release/bench_lob
