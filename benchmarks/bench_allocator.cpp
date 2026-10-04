#include <benchmark/benchmark.h>
#include "fixed_block_allocator.hpp"
#include <memory>
#include <vector>

struct OrderEvent {
    uint64_t order_id;
    double price;
    uint32_t qty;
    char side;
    char padding[35]; // Enforce 64-byte aligned structure
};

static void BM_StandardHeapAllocation(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<OrderEvent*> ptrs;
        ptrs.reserve(1024);

        for (size_t i = 0; i < 1024; ++i) {
            ptrs.push_back(new OrderEvent{i, 100.50, 100, 'B', {}});
        }

        for (auto* ptr : ptrs) {
            delete ptr;
        }
    }
}
BENCHMARK(BM_StandardHeapAllocation);

static void BM_FixedBlockAllocator(benchmark::State& state) {
    FixedBlockAllocator<OrderEvent, 1024> pool;

    for (auto _ : state) {
        std::vector<OrderEvent*> ptrs;
        ptrs.reserve(1024);

        for (size_t i = 0; i < 1024; ++i) {
            ptrs.push_back(pool.allocate(OrderEvent{i, 100.50, 100, 'B', {}}));
        }

        for (auto* ptr : ptrs) {
            pool.deallocate(ptr);
        }
    }
}
BENCHMARK(BM_FixedBlockAllocator);

BENCHMARK_MAIN();
