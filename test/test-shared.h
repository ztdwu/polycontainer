#include "test-polycontainer.h"
#include "../contiguous-polycontainer.hpp"
#include "../polycontainer.hpp"

#include <algorithm>
#include <vector>

#include <catch.hpp>


using polycontainer::PolyContainer;
using polycontainer::ContiguousPolyContainer;

struct Base {
    virtual ~Base() = default;
    virtual int get() const = 0;
};

struct D1 : Base { int get() const override { return 1; } };
struct D2 : Base { int get() const override { return 2; } };
struct D3 : Base { int get() const override { return 3; } };


/** Test factory to avoid duplicating tests for the two containers */
template <typename Container>
class Test {

/** The required tests */
public:
    static void ctor() {
        make_container();
    }

    static void move_ctor() {
        {
            auto container = make_container();
            Container{ std::move(container) };
        }
        {
            auto container = make_container();
            container.push_back(make_derived<D1>());
            Container{ std::move(container) };
        }
    }

    static void push_back() {
        auto container = make_container();
        container.push_back(make_derived<D1>());
    };

    static auto empty() -> bool {
        auto container = make_container();
        return container.len() == 0u;
    }

    static auto len() -> bool {
        auto container = make_container();
        container.push_back(make_derived<D1>());
        return container.len() == 1u;
    }

    static auto for_each() -> bool {
        auto container = make_container();
        container.push_back(make_derived<D1>());
        container.push_back(make_derived<D2>());
        container.push_back(make_derived<D3>());

        auto seen = std::vector<int>{ };
        container.for_each([&seen](const auto &item) {
            seen.push_back(item.get());
        });

        std::sort(seen.begin(), seen.end());
        const int expected[] = { 1, 2, 3 };
        return std::equal(seen.begin(), seen.end(), expected);
    }


/** Factory functions */
private:
    static auto make_container() {
        return Container{ };
    }

    template <typename Derived>
    static typename std::enable_if<std::is_same<Container, PolyContainer<Base>>::value,
                                   std::unique_ptr<Derived>>::type
    make_derived() {
        return std::make_unique<Derived>();
    }

    template <typename Derived>
    static typename std::enable_if<std::is_same<Container, ContiguousPolyContainer<Base>>::value,
                                   Derived>::type
    make_derived() {
        return Derived{ };
    }
};
