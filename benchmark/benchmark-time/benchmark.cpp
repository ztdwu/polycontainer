#include "contiguous-polycontainer.hpp"
#include "polycontainer.hpp"
#include "object-factory.h"

#include "benchmark/benchmark.h"

template <typename Container>
static void run_benchmark(benchmark::State& state) {
    using Factory = ContainerFactory<Container>;
    while (state.KeepRunning()) {

        state.PauseTiming();
        const auto container = Factory::generate_container(state.range_x());

        state.ResumeTiming();
        Factory::do_work(container);
    }
}

enum iterations {
    min = 1 << 4,
    max = 1 << 20
};

const auto std_vector_              = run_benchmark<StdVectorContainer>;
const auto PolyContainer_           = run_benchmark<PolyContainer>;
const auto ContiguousPolyContainer_ = run_benchmark<ContiguousPolyContainer>;

BENCHMARK(std_vector_)             ->Range(min, max);
BENCHMARK(PolyContainer_)          ->Range(min, max);
BENCHMARK(ContiguousPolyContainer_)->Range(min, max);

BENCHMARK_MAIN()
