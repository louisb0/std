#include "vector.hpp"

#include <benchmark/benchmark.h>

static void BM_VectorTest(benchmark::State &state) {
    for (auto _ : state) {
        mystl::vector::test();
    }
}
BENCHMARK(BM_VectorTest);

BENCHMARK_MAIN();
