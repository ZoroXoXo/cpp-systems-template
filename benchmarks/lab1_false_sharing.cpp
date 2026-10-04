#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>
#include <vector>
#include <new>

// Unpadded: Both counters live on the EXACT SAME 64-byte cache line.
struct FalseSharingStruct {
    std::atomic<uint64_t> thread1_counter{0}; // 8 bytes
    std::atomic<uint64_t> thread2_counter{0}; // 8 bytes (Offset +8 bytes -> Same Cache Line)
};

// Padded: Forces each counter onto its OWN 64-byte cache line.
struct alignas(std::hardware_destructive_interference_size) TrueSharingAvoided {
    std::atomic<uint64_t> counter{0};
};

struct PaddedStruct {
    TrueSharingAvoided thread1_counter; // Cache Line 1 (64 bytes)
    TrueSharingAvoided thread2_counter; // Cache Line 2 (64 bytes)
};

static void BM_FalseSharing(benchmark::State& state) {
    FalseSharingStruct data;

    for (auto _ : state) {
        std::thread t1([&]() {
            for (int i = 0; i < 1'000'000; ++i) {
                data.thread1_counter.fetch_add(1, std::memory_order_relaxed);
            }
        });

        std::thread t2([&]() {
            for (int i = 0; i < 1'000'000; ++i) {
                data.thread2_counter.fetch_add(1, std::memory_order_relaxed);
            }
        });

        t1.join();
        t2.join();
    }
}
BENCHMARK(BM_FalseSharing)->UseRealTime();

static void BM_TrueSharingAvoided(benchmark::State& state) {
    PaddedStruct data;

    for (auto _ : state) {
        std::thread t1([&]() {
            for (int i = 0; i < 1'000'000; ++i) {
                data.thread1_counter.counter.fetch_add(1, std::memory_order_relaxed);
            }
        });

        std::thread t2([&]() {
            for (int i = 0; i < 1'000'000; ++i) {
                data.thread2_counter.counter.fetch_add(1, std::memory_order_relaxed);
            }
        });

        t1.join();
        t2.join();
    }
}
BENCHMARK(BM_TrueSharingAvoided)->UseRealTime();

BENCHMARK_MAIN();
