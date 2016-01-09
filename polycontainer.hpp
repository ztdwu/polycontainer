#pragma once

#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace polycontainer {


template <typename Base>
class PolyContainer {

public:
    PolyContainer()                 = default;
    PolyContainer(PolyContainer &&) = default;
    ~PolyContainer()                = default;

    PolyContainer(const PolyContainer &)            = delete;
    PolyContainer &operator=(const PolyContainer &) = delete;


    template <typename Derived>
    void push_back(std::unique_ptr<Derived> d) {
        static_assert(std::is_base_of<Base, Derived>::value, "Cannot insert an object that does not derive from the base.");
        segments[typeid(d)].push_back(std::move(d));
    }

    template <typename Func>
    void for_each(Func &&f) {
        for ( const auto &segment : segments ) {
            for ( const auto &item : segment.second ) {
                f(*item);
            }
        }
    }

    template <typename Func>
    void for_each(Func &&f) const {
        const_cast<PolyContainer &>(*this).for_each(std::forward<Func>(f));
    }


private:
    using Segment = std::vector<std::unique_ptr<Base>>;
    std::unordered_map<std::type_index, Segment> segments;
};

}
