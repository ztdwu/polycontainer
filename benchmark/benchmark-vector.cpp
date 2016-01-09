#include "benchmark.hpp"

const auto benchmark_std_vector = benchmark_container<std::vector<std::unique_ptr<Base>>>;

BENCHMARK(benchmark_std_vector)->Range(1 << 5, 1 << 13);

BENCHMARK_MAIN();
