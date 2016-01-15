#include "test-contiguous-polycontainer.h"
#include "test-shared.h"

#include "../contiguous-polycontainer.hpp"

#include <catch.hpp>


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
