#include <benchmark/benchmark.h>
#include "spsc_queue.hpp"
#include <thread>

static void BM_SPSCQueueThroughput(benchmark::State& state) {
    SPSCQueue<uint64_t, 1024> queue;
    const uint64_t num_items = 1'000'000;

    for (auto _ : state) {
        std::thread producer([&]() {
            for (uint64_t i = 0; i < num_items; ++i) {
                while (!queue.emplace(i)) {
                    // Spin-wait if full
                }
            }
        });

        std::thread consumer([&]() {
            for (uint64_t i = 0; i < num_items; ++i) {
                std::optional<uint64_t> item;
                while (!(item = queue.pop())) {
                    // Spin-wait if empty
                }
            }
        });

        producer.join();
        consumer.join();
    }
}
BENCHMARK(BM_SPSCQueueThroughput)->UseRealTime();

BENCHMARK_MAIN();
