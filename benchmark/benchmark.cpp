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

#include "benchmark/benchmark.h"


struct Base {
    virtual ~Base() { }
    virtual int get() const = 0;
};

struct D1 : Base { int get() const override { return 1; } };
struct D2 : Base { int get() const override { return 2; } };
struct D3 : Base { int get() const override { return 3; } };
struct D4 : Base { int get() const override { return 4; } };
struct D5 : Base { int get() const override { return 5; } };
struct D6 : Base { int get() const override { return 6; } };


using StdVectorContainer      = std::vector<std::unique_ptr<Base>>;
using PolyContainer           = polycontainer::PolyContainer<Base>;
using ContiguousPolyContainer = polycontainer::ContiguousPolyContainer<Base>;


template <typename Container>
class Bench {

public:
    static void benchmark(benchmark::State& state) {
        while (state.KeepRunning()) {

            state.PauseTiming();
            const auto container = generate_container(state.range_x());

            state.ResumeTiming();
            do_work(container);
        }
    }

private:
    static auto generate_container(const int size) {
        auto container    = Container{ };
        auto generator    = std::mt19937{ std::random_device{ }() };
        auto distribution = std::uniform_int_distribution<>(1, 6);

        for ( auto _ = 0; _ < size; _++ ) {
            const auto id = distribution(generator);
            switch ( id ) {
                case 1 : insert<D1>(container); break;
                case 2 : insert<D2>(container); break;
                case 3 : insert<D3>(container); break;
                case 4 : insert<D4>(container); break;
                case 5 : insert<D5>(container); break;
                case 6 : insert<D6>(container); break;
                default : assert(false && "unreachable");
            }
        }
        return container;
    }

    template <typename Derived>
    static auto create_derived() ->
        std::enable_if_t<std::is_same<Container, ContiguousPolyContainer>::value, Derived>
    {
        return Derived{ };
    }

    template <typename Derived>
    static auto create_derived() ->
        std::enable_if_t<not std::is_same<Container, ContiguousPolyContainer>::value, std::unique_ptr<Derived>>
    {
        return std::make_unique<Derived>();
    }

    template <typename Derived>
    static void insert(Container &container) {
        container.push_back(create_derived<Derived>());
    }

    static void do_work(const Container &c) {
        c.for_each([](const auto &item) {
            item.get();
        });
    }
};

template <>
void Bench<StdVectorContainer>::do_work(const StdVectorContainer &c) {
    for ( const auto &item : c ) {
        item->get();
    }
}


enum iterations {
    min = 1 << 4,
    max = 1 << 20
};

const auto std_vector               = Bench<StdVectorContainer>     ::benchmark;
const auto polycontainer_pointers   = Bench<PolyContainer>          ::benchmark;
const auto polycontainer_contiguous = Bench<ContiguousPolyContainer>::benchmark;

BENCHMARK(std_vector)              ->Range(min, max);
BENCHMARK(polycontainer_pointers)  ->Range(min, max);
BENCHMARK(polycontainer_contiguous)->Range(min, max);

BENCHMARK_MAIN()
