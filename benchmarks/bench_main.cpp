#include <benchmark/benchmark.h>
#include <vector>
#include <list>
#include <numeric>

constexpr size_t N = 100'000;

static void BM_VectorIteration(benchmark::State& state) {
    std::vector<int> v(N);
    std::iota(v.begin(), v.end(), 1);

    for (auto _ : state) {
        int64_t sum = 0;
        for (const auto& val : v) {
            sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_VectorIteration);

static void BM_ListIteration(benchmark::State& state) {
    std::list<int> l;
    for (size_t i = 0; i < N; ++i) {
        l.push_back(static_cast<int>(i));
    }

    for (auto _ : state) {
        int64_t sum = 0;
        for (const auto& val : l) {
            sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_ListIteration);

BENCHMARK_MAIN();
