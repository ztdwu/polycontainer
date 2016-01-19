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

const auto std_vector               = run_benchmark<StdVectorContainer>;
const auto polycontainer_pointers   = run_benchmark<PolyContainer>;
const auto polycontainer_contiguous = run_benchmark<ContiguousPolyContainer>;

BENCHMARK(std_vector)              ->Range(min, max);
BENCHMARK(polycontainer_pointers)  ->Range(min, max);
BENCHMARK(polycontainer_contiguous)->Range(min, max);

BENCHMARK_MAIN()
