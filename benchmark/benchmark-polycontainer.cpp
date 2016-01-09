#include "benchmark.hpp"

const auto benchmark_polycontainer = benchmark_container<PolyContainer<Base>>;

BENCHMARK(benchmark_polycontainer)->Range(1 << 5, 1 << 13);

BENCHMARK_MAIN();
