#pragma once

#include <contiguous-polycontainer.hpp>
#include <polycontainer.hpp>

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
            insert<D1>(container);
            Container{ std::move(container) };
        }
    }

    static void push_back() {
        auto container = make_container();
        insert<D1>(container);
    };

    static auto for_each() -> bool {
        auto container = make_container();
        insert<D1>(container);
        insert<D2>(container);
        insert<D3>(container);

        const auto container_const = std::move(container);
        auto seen = std::vector<int>{ };
        container_const.for_each([&seen](const auto &item) {
            seen.push_back(item.get());
        });

        std::sort(seen.begin(), seen.end());
        const int expected[] = { 1, 2, 3 };
        return std::equal(seen.begin(), seen.end(), expected);
    }

    static auto get_segment_empty() -> bool {
        auto container = make_container();
        return container.template get_segment<D1>().size() == 0u;
    }

    static auto get_segment() -> bool {
        auto container = make_container();
        insert<D1>(container);
        return container.template get_segment<D1>().size() == 1u;
    }

    static auto empty() -> bool {
        auto container = make_container();
        return container.len() == 0u;
    }

    static auto len() -> bool {
        auto container = make_container();
        insert<D1>(container);
        return container.len() == 1u;
    }

    static auto clear_empty() -> bool {
        auto container = make_container();
        container.clear();
        return container.len() == 0u;
    }

    static auto clear() -> bool {
        auto container = make_container();
        insert<D1>(container);
        insert<D2>(container);
        container.clear();
        return container.len() == 0u;
    }


/** Factory functions */
public:
    static auto make_container() {
        return Container{ };
    }

    template <typename Derived>
    static auto make_derived() ->
        std::enable_if_t<std::is_same<Container, PolyContainer<Base>>::value, std::unique_ptr<Derived>>
    {
        return std::make_unique<Derived>();
    }

    template <typename Derived>
    static auto make_derived() ->
        std::enable_if_t<std::is_same<Container, ContiguousPolyContainer<Base>>::value, Derived>
    {
        return Derived{ };
    }

    template <typename Derived>
    static auto insert(Container &container) {
        container.push_back(make_derived<Derived>());
    }

};
