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
    PolyContainer() noexcept = default;
    ~PolyContainer()         = default;

    PolyContainer(PolyContainer &&) noexcept        = default;
    PolyContainer(const PolyContainer &)            = delete;
    PolyContainer& operator=(const PolyContainer &) = delete;


/** Public methods for insertion and iteration */
public:
    template <typename Derived>
    auto& push_back(std::unique_ptr<Derived> item) {
        static_assert(std::is_base_of<Base, Derived>::value,
                      "Cannot insert an object that does not derive from the base.");

        auto &vec = segments[typeid(Derived)];
        vec.push_back(std::move(item));

        length += 1;
        return vec.back();
    }

    template <typename Func>
    void for_each(const Func &f) {
        for ( auto &keyval : segments ) {
            for ( auto &item : keyval.second ) {
                f(*item);
            }
        }
    }

    template <typename Func>
    void for_each(const Func &f) const {
        const_cast<PolyContainer &>(*this).for_each(f);
    }

    auto len() const -> size_t {
        return length;
    }


/** Data members */
private:
    using Segment = std::vector<std::unique_ptr<Base>>;
    std::unordered_map<std::type_index, Segment> segments;
    size_t length = 0;
};

}
