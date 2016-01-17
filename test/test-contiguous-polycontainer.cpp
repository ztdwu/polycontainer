#include "test-contiguous-polycontainer.h"
#include "test-shared.h"

#include "../contiguous-polycontainer.hpp"

#include "catch.hpp"


using Tester = Test<ContiguousPolyContainer<Base>>;

TEST_CASE( "contiguous default ctor", "[ContiguousPolyContainer]" ) {
    REQUIRE_NOTHROW( Tester::ctor() );
}

TEST_CASE( "contiguous move ctor", "[ContiguousPolyContainer]" ) {
    REQUIRE_NOTHROW( Tester::move_ctor() );
}

TEST_CASE( "contiguous push_back", "[ContiguousPolyContainer]" ) {
    REQUIRE_NOTHROW( Tester::push_back() );
}

TEST_CASE( "contiguous empty", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::empty() );
}

TEST_CASE( "contiguous len", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::len() );
}

TEST_CASE( "contiguous for_each", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::for_each() );
}

TEST_CASE( "contiguous get_segment_empty", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::get_segment_empty() );
}

TEST_CASE( "contiguous get_segment", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::get_segment() );
}

TEST_CASE( "contiguous clear_empty", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::clear_empty() );
}

TEST_CASE( "contiguous clear", "[ContiguousPolyContainer]" ) {
    REQUIRE( Tester::clear() );
}

TEST_CASE( "contiguous by ref push_back", "[ContiguousPolyContainer]" ) {
    auto container = ContiguousPolyContainer<Base>{ };

    auto item1 = D1{ };
    REQUIRE_NOTHROW( container.push_back(item1) );

    const auto item2 = D1{ };
    REQUIRE_NOTHROW( container.push_back(item2) );

    REQUIRE( container.len() == 2u );

    const auto &vec = container.get_segment<D1>();
    REQUIRE( vec.size() == container.len() );
}