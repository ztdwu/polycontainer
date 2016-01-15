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
    ContiguousPolyContainer() noexcept = default;
    ~ContiguousPolyContainer()         = default;

    ContiguousPolyContainer(ContiguousPolyContainer &&) noexcept        = default;
    ContiguousPolyContainer(const ContiguousPolyContainer &)            = delete;
    ContiguousPolyContainer &operator=(const ContiguousPolyContainer &) = delete;


/** Public methods for insertion and iteration */
public:
    template <typename Derived>
    auto& push_back(Derived &&d) {
        auto &segment = get_segment<Derived>();
        segment.push_back(std::forward<Derived>(d));

        length += 1;
        return segment.back();
    }

    template <typename Func>
    void for_each(const Func &f) {
        for ( auto &keyval : segments ) {
            auto &segment = *keyval.second;
            segment.for_each(f);
        }
    }

    template <typename Func>
    void for_each(const Func &f) const {
        const_cast<ContiguousPolyContainer &>(*this).for_each(f);
    }

    template <typename Derived>
    auto& get_segment() {
        static_assert(std::is_base_of<Base, std::remove_reference_t<Derived>>::value,
            "Cannot insert an object that does not derive from the base.");

        auto &segment = segments[typeid(Derived)];
        if ( !segment ) {
            segment.reset(new Segment<Derived>);
        }
        return static_cast<Segment<Derived> &>(*segment).vec;
    }

    auto len() const -> size_t {
        return length;
    }

    void clear() {
        segments.clear();
        length = 0u;
    }


/** Data members */
private:
    using Pointer = std::unique_ptr<SegmentI>;
    std::unordered_map<std::type_index, Pointer> segments;
    size_t length = 0u;


/** Private helper classes */
private:

    /** Helper interface for contiguous polymorphic storage */
    class SegmentI {
    public:
        virtual ~SegmentI() = default;

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

        void for_each(const std::function<void(Base &)> &f) override {
            for ( auto &item : vec ) {
                f(item);
            }
        }

        void for_each(const std::function<void(Base &)> &f) const override {
            const_cast<Segment<Derived> &>(*this).for_each(f);
        }


    public:
        std::vector<Derived> vec;
    };
};

}
