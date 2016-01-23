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


/** ctors and dtor*/
public:
    ContiguousPolyContainer() noexcept = default;
    ~ContiguousPolyContainer()         = default;

    ContiguousPolyContainer(ContiguousPolyContainer &&) noexcept            = default;
    ContiguousPolyContainer& operator=(ContiguousPolyContainer &&) noexcept = default;

    ContiguousPolyContainer(const ContiguousPolyContainer &)            = delete;
    ContiguousPolyContainer& operator=(const ContiguousPolyContainer &) = delete;


/** Forward declaration of storage classes and exceptions */
private:
    class Segment;

    template <typename Derived>
    class SegmentImpl;

/** This exception is thrown if an object isn't inserted by its most derived type */
public:
    class BadDerivedTypeException : public std::exception {
    public:
        BadDerivedTypeException(const std::type_info &variable_type, const std::type_info &template_type)
            : variable_type_(variable_type)
            , template_type_(template_type)
            , msg(std::string{ "The most-derived type of the variable argument is " } + variable_type_.name() +
                  ", but the type of the template argument is " + template_type_.name() +
                  ".\nThe type of the template argument much match the most derived type of" +
                  " the variable argument, otherwise it may cause slicing.")
        { }

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        const std::type_index variable_type_;
        const std::type_index template_type_;
        const std::string msg;
    };

/** Public methods for insertion and iteration */
public:
    template <typename Derived>
    auto& push_back(Derived &&d) {
        if ( typeid(d) != typeid(Derived) ) {
            throw BadDerivedTypeException(typeid(d), typeid(Derived));
        }

        auto &segment = get_segment<std::decay_t<Derived>>();
        segment.push_back(std::forward<Derived>(d));
        return segment.back();
    }

    template <typename Derived>
    auto& push_back(const Derived &d) {
        return push_back(const_cast<Derived &>(d));
    }

    template <typename Func>
    void for_each(const Func &f) {
        for ( auto &keyval : segments_ ) {
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
        static_assert(std::is_base_of<Base, Derived>::value,
            "The function template argument must derive from the class template argument.");

        auto &segment = segments_[typeid(Derived)];
        if ( !segment ) {
            segment = std::make_unique<SegmentImpl<Derived>>();
        }
        return static_cast<SegmentImpl<Derived> &>(*segment).vec_;
    }

    auto len() const {
        size_t length = 0u;
        for ( const auto &keyval : segments_ ) {
            length += keyval.second->len();
        }
        return length;
    }

    void clear() {
        segments_.clear();
    }


/** Data members */
private:
    using Pointer = std::unique_ptr<Segment>;
    std::unordered_map<std::type_index, Pointer> segments_;


/** Private helper classes */
private:

    /** Helper interface for contiguous polymorphic storage */
    class Segment {
    public:
        virtual ~Segment() = default;

        virtual auto len() const -> size_t = 0;
        virtual void for_each(const std::function<void(Base &)> &) = 0;
    };

    /** SegmentImpl must be separated from its interface
     *  in order to achieve contiguous storage
     */
    template <typename Derived>
    class SegmentImpl : public Segment {
    public:
        SegmentImpl()               noexcept = default;
        SegmentImpl(SegmentImpl &&) noexcept = default;

        auto len() const -> size_t override {
            return vec_.size();
        }

        void for_each(const std::function<void(Base &)> &f) override {
            for ( auto &item : vec_ ) {
                f(item);
            }
        }

    public:
        std::vector<Derived> vec_;
    };
};

}
