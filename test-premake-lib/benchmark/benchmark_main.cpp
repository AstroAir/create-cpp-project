#include <benchmark/benchmark.h>
#include "test-premake-lib/test-premake-lib.h"

// 简单的add函数基准测试
static void BM_Add(benchmark::State& state) {
  // 这段代码会被计时
  for (auto _ : state) {
    benchmark::DoNotOptimize(test-premake-lib::add(42, 24));
  }
}
BENCHMARK(BM_Add);

// 使用不同输入值的基准测试
static void BM_Add_WithArgs(benchmark::State& state) {
  const int a = state.range(0);
  const int b = state.range(1);
  for (auto _ : state) {
    benchmark::DoNotOptimize(test-premake-lib::add(a, b));
  }
}
// 测试不同输入组合
BENCHMARK(BM_Add_WithArgs)->Args({1, 1})->Args({10, 10})->Args({100, 100});

// 测试Example类
static void BM_ExampleGetValue(benchmark::State& state) {
  test-premake-lib::Example example(state.range(0));
  for (auto _ : state) {
    benchmark::DoNotOptimize(example.getValue());
  }
}
BENCHMARK(BM_ExampleGetValue)->Range(8, 8<<10);

BENCHMARK_MAIN();
