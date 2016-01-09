#pragma once

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

template <typename Container>
void populate_container(Container &c, const int size) {
    auto generator    = std::mt19937_64 { std::random_device{ }() };
    auto distribution = std::uniform_int_distribution<>(1, 6);

    for ( auto _ = 0; _ < size; _++ ) {
        const auto id = distribution(generator);
        switch ( id ) {
            case 1 : c.push_back(std::make_unique<Derived1>()); break;
            case 2 : c.push_back(std::make_unique<Derived2>()); break;
            case 3 : c.push_back(std::make_unique<Derived3>()); break;
            case 4 : c.push_back(std::make_unique<Derived4>()); break;
            case 5 : c.push_back(std::make_unique<Derived5>()); break;
            case 6 : c.push_back(std::make_unique<Derived6>()); break;
            default : assert(false && "unreachable");
        }
    }
}

template <typename Container>
void do_work(const Container &c) {
    std::for_each(c.cbegin(), c.cend(), [](const auto &item) {
        item->compute();
    });
}

template<>
void do_work(const PolyContainer<Base> &c) {
    c.for_each([](const Base &item) {
        item.compute();
    });
}

template<typename Container>
void benchmark_container(benchmark::State& state) {
    while (state.KeepRunning()) {
        state.PauseTiming();
        auto container = create_container<Container>();
        populate_container(container, state.range_x());

        state.ResumeTiming();
        do_work(container);
    }
}
