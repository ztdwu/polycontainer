#include "contiguous-polycontainer.hpp"
#include "polycontainer.hpp"

#include "../object-factory.h"

template <typename Container>
void cache_test(const int size) {
    using Factory = ContainerFactory<Container>;
    const auto container = Factory::generate_container(size);
    Factory::do_work(container);
}

int main(const int argc, const char *const argv[]) {
    assert(argc == 3);

    const auto type = std::string{ argv[1] };
    const auto size = atoi(argv[2]);

    assert(size > 0);

    if ( type == "vector" )
        cache_test<StdVectorContainer>(size);

    else if ( type == "polycontainer" )
        cache_test<PolyContainer>(size);

    else if ( type == "contiguous-polycontainer" )
        cache_test<ContiguousPolyContainer>(size);

    else
        assert(false && "invalid container");
}
