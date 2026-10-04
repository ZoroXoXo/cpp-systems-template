#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>

constexpr int ITERATIONS = 10'000'000;

static void BM_SeqCst(benchmark::State& state) {
    std::atomic<uint64_t> counter{0};
    for (auto _ : state) {
        std::thread t1([&]() {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_seq_cst);
            }
        });
        std::thread t2([&]() {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_seq_cst);
            }
        });
        t1.join();
        t2.join();
    }
}
BENCHMARK(BM_SeqCst)->UseRealTime();

static void BM_AcqRel(benchmark::State& state) {
    std::atomic<uint64_t> counter{0};
    for (auto _ : state) {
        std::thread t1([&]() {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_acq_rel);
            }
        });
        std::thread t2([&]() {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_acq_rel);
            }
        });
        t1.join();
        t2.join();
    }
}
BENCHMARK(BM_AcqRel)->UseRealTime();

static void BM_Relaxed(benchmark::State& state) {
    std::atomic<uint64_t> counter{0};
    for (auto _ : state) {
        std::thread t1([&]() {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
        std::thread t2([&]() {
            for (int i = 0; i < ITERATIONS; ++i) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
        t1.join();
        t2.join();
    }
}
BENCHMARK(BM_Relaxed)->UseRealTime();

BENCHMARK_MAIN();
