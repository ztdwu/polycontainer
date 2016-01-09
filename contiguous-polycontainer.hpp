#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace polycontainer {

template <typename Base>
class ContiguousPolyContainer {

/** Forward declaration of storage classes */
private:
    class SegmentI;
    template <typename Derived> class Segment;


/** ctors and dtor*/
public:
    ContiguousPolyContainer()  = default;
    ~ContiguousPolyContainer() = default;

    ContiguousPolyContainer(ContiguousPolyContainer &&) noexcept        = default;
    ContiguousPolyContainer(const ContiguousPolyContainer &)            = delete;
    ContiguousPolyContainer &operator=(const ContiguousPolyContainer &) = delete;


/** Public methods for insertion and iteration */
public:
    template <typename Derived>
    inline void push_back(const Derived &d) {
        get_segment(d)->push_back(d);
    }

    template <typename Derived>
    inline void push_back(Derived &&d) {
        get_segment(d);
        get_segment(d)->push_back(std::forward<Derived>(d));
    }

    template <typename Func>
    inline void for_each(const Func &f) {
        for ( auto &keyval : segments ) {
            auto &segment = *keyval.second;
            segment.for_each(f);
        }
    }

    template <typename Func>
    inline void for_each(const Func &f) const {
        const_cast<ContiguousPolyContainer &>(*this).for_each(f);
    }

/** Private methods */
private:
    template <typename Derived>
    inline auto &get_segment(const Derived &d) {
        static_assert(std::is_base_of<Base, std::remove_reference_t<Derived>>::value,
            "Cannot insert an object that does not derive from the base.");

        auto &segment = segments[typeid(d)];
        if ( !segment ) {
            segment.reset(new Segment<Derived>);
        }
        return segment;
    }


/** Data members */
private:
    using Pointer = std::unique_ptr<SegmentI>;
    std::unordered_map<std::type_index, Pointer> segments;


/** Private helper classes */
private:

    /** Helper interface for contiguous polymorphic storage */
    class SegmentI {
    public:
        virtual ~SegmentI() = default;

        virtual void push_back(const Base &)                             = 0;
        virtual void push_back(Base &&)                                  = 0;
        virtual void for_each(const std::function<void(Base &)> &)       = 0;
        virtual void for_each(const std::function<void(Base &)> &) const = 0;
    };

    /** Segment impl must be separated from its interface
     *  in order to achieve contiguous storage
     */
    template <typename Derived>
    class Segment : public SegmentI {
    public:
        Segment()           noexcept = default;
        Segment(Segment &&) noexcept = default;

        void push_back(const Base &base) override {
            const auto &item = static_cast<const Derived &>(base);
            vec.push_back(item);
        }

        void push_back(Base &&base) override {
            auto &&item = static_cast<Derived &&>(base);
            vec.push_back(std::move(item));
        }

        void for_each(const std::function<void(Base &)> &f) override {
            for ( auto &item : vec ) {
                f(item);
            }
        }

        void for_each(const std::function<void(Base &)> &f) const override {
            const_cast<Segment<Derived> &>(*this).for_each(f);
        }

    private:
        std::vector<Derived> vec;
    };
};

}
