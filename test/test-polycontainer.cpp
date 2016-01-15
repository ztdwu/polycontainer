#include "test-polycontainer.h"
#include "test-shared.h"

#include "../polycontainer.hpp"

#include <catch.hpp>


using Tester = Test<PolyContainer<Base>>;

TEST_CASE( "non-contiguous default ctor", "[PolyContainer]" ) {
    REQUIRE_NOTHROW( Tester::ctor() );
}

TEST_CASE( "non-contiguous move ctor", "[PolyContainer]" ) {
    REQUIRE_NOTHROW( Tester::move_ctor() );
}

TEST_CASE( "non-contiguous push_back", "[PolyContainer]" ) {
    REQUIRE_NOTHROW( Tester::push_back() );
}

TEST_CASE( "non-contiguous empty", "[PolyContainer]" ) {
    REQUIRE( Tester::empty() );
}

TEST_CASE( "non-contiguous len", "[PolyContainer]" ) {
    REQUIRE( Tester::len() );
}

TEST_CASE( "non-contiguous for_each", "[PolyContainer]" ) {
    REQUIRE( Tester::for_each() );
}

TEST_CASE( "non-contiguous get_segment_empty", "[PolyContainer]" ) {
    REQUIRE( Tester::get_segment_empty() );
}

TEST_CASE( "non-contiguous get_segment", "[PolyContainer]" ) {
    REQUIRE( Tester::get_segment() );
}

