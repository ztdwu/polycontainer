#pragma once

#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace polycontainer {

template <typename Base>
class PolyContainer {

/** ctors and dtor*/
public:
    PolyContainer()  = default;
    ~PolyContainer() = default;

    PolyContainer(PolyContainer &&) noexcept        = default;
    PolyContainer(const PolyContainer &)            = delete;
    PolyContainer &operator=(const PolyContainer &) = delete;


/** Public methods for insertion and iteration */
public:
    template <typename Derived>
    inline void push_back(std::unique_ptr<Derived> item) {
        static_assert(std::is_base_of<Base, Derived>::value, "Cannot insert an object that does not derive from the base.");
        segments[typeid(Derived)].push_back(std::move(item));
    }

    template <typename Func>
    inline void for_each(const Func &f) {
        for ( const auto &segment : segments ) {
            for ( const auto &item : segment.second ) {
                f(*item);
            }
        }
    }

    template <typename Func>
    inline void for_each(const Func &f) const {
        const_cast<PolyContainer &>(*this).for_each(f);
    }


/** Data members */
private:
    using Segment = std::vector<std::unique_ptr<Base>>;
    std::unordered_map<std::type_index, Segment> segments;
};

}
