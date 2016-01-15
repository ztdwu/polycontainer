#include "../contiguous-polycontainer.hpp"
#include "../polycontainer.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

// Requires google's benchmark framework https://github.com/google/benchmark
// Must compile with -lpthread
#include <benchmark/benchmark.h>

using polycontainer::PolyContainer;
using polycontainer::ContiguousPolyContainer;

struct Base {
    virtual ~Base() { }
    virtual int compute() const = 0;
};

struct Derived1 : Base { int compute() const override { return 1; } };
struct Derived2 : Base { int compute() const override { return 2; } };
struct Derived3 : Base { int compute() const override { return 3; } };
struct Derived4 : Base { int compute() const override { return 4; } };
struct Derived5 : Base { int compute() const override { return 5; } };
struct Derived6 : Base { int compute() const override { return 6; } };

template<typename Container>
auto create_container() {
    return Container{ };
}

template<typename Container, typename Derived>
auto create_derived() ->
    typename std::enable_if<std::is_same<Container, ContiguousPolyContainer<Base>>::value,
                            Derived>::type
{
    return Derived{ };
}

template<typename Container, typename Derived>
auto create_derived() ->
    typename std::enable_if<not std::is_same<Container, ContiguousPolyContainer<Base>>::value,
                            std::unique_ptr<Derived>>::type
{
    return std::make_unique<Derived>();
}

template <typename Container>
auto generate_container(const int size) {
    auto container    = create_container<Container>();
    auto generator    = std::mt19937_64 { std::random_device{ }() };
    auto distribution = std::uniform_int_distribution<>(1, 6);

    for ( auto _ = 0; _ < size; _++ ) {
        const auto id = distribution(generator);
        switch ( id ) {
            case 1 : container.push_back(create_derived<Container, Derived1>()); break;
            case 2 : container.push_back(create_derived<Container, Derived2>()); break;
            case 3 : container.push_back(create_derived<Container, Derived3>()); break;
            case 4 : container.push_back(create_derived<Container, Derived4>()); break;
            case 5 : container.push_back(create_derived<Container, Derived5>()); break;
            case 6 : container.push_back(create_derived<Container, Derived6>()); break;
            default : assert(false && "unreachable");
        }
    }
    return container;
}

template <typename Container>
void do_work(const Container &c) {
    c.for_each([](const Base &item) {
        item.compute();
    });
}

template <>
void do_work(const std::vector<std::unique_ptr<Base>> &c) {
    std::for_each(c.cbegin(), c.cend(), [](const auto &item) {
        item->compute();
    });
}

template<typename Container>
void benchmark_container(benchmark::State& state) {
    while (state.KeepRunning()) {

        state.PauseTiming();
        const auto container = generate_container<Container>(state.range_x());

        state.ResumeTiming();
        do_work(container);
    }
}


enum iterations {
    min = 1 << 5,
    max = 1 << 20
};

const auto std_vector               = benchmark_container<std::vector<std::unique_ptr<Base>>>;
const auto polycontainer_pointers   = benchmark_container<PolyContainer<Base>>;
const auto polycontainer_contiguous = benchmark_container<ContiguousPolyContainer<Base>>;

BENCHMARK(std_vector)              ->Range(min, max);
BENCHMARK(polycontainer_pointers)  ->Range(min, max);
BENCHMARK(polycontainer_contiguous)->Range(min, max);

BENCHMARK_MAIN()
