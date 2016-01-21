# PolyContainer [![Build Status](https://travis-ci.org/ztdwu/polycontainer.svg?branch=master)](https://travis-ci.org/ztdwu/polycontainer) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE?raw=true)
Containers of polymorphic types can often be slow to iterate because the processor has to branch-predict which virtual function implementation to call. As well, polymorphics types are usually stored non-contiguously through pointers, which is severely cache-unfriendly as it hampers the processor's ability to predict and prefetch the next block of data. PolyContainer is a container based on the ideas in an excellent [blog post](http://bannalia.blogspot.ca/2014/05/fast-polymorphic-collections.html) (a must read) by Joaquín M López Muñoz. It stores objects of each derived type contiguously in its own bucket.

This library offers two types of containers: `ContinuousPolyContainer` and `PolyContainer`.
- `ContinuousPolyContainer` lays out each object contiguously in memory, offering the best cache locality as well as branch predictability. 
- `PolyContainer` holds smart pointers to each element in the container, which loses the huge benefit of cache locality (and can be quite slow for large container sizes, see benchmarks below). However, this allows for easier deletions of individual objects in the container while retaining the benefits of being branch-predictor friendly.

## Benchmarks (linear iteration):
These microbenchmarks are compiled with Clang 3.6 on Linux 4.3.3 x86_64 (8 X 2400.09 MHz CPU s)

![Benchmarks Graph](/benchmark/benchmarks.png?raw=true)

`ContiguousPolyContainer` appears to be easily the fastest, and seems scale linearly with the number of elements.

The non-contiguous `PolyContainer` is a significantly faster than `std::vector` up until around 32k elements, after which its performance starts to degrade severely due to a high number of cache misses (even more than `std::vector`!), so use with caution.


## Mini Documentation:
```c++
// inserts and returns a reference to the inserted value
container.push_back(item);

// calls the lambda for each item in the container
container.for_each([](auto &item) { });

// returns a reference to the underlying vector that holds all items of DerivedType
container.get_segment<DerivedType>();

// returns the total number of items in the container
container.len();

// removes all items
container.clear();
```

## Example:
```c++
#include "polycontainer.hpp"
#include <iostream>
using polycontainer::ContiguousPolyContainer;

struct Base {
    virtual ~Base() { }
    virtual int compute() const = 0;
};

struct D1 : Base { int compute() const override { return 1; } };
struct D2 : Base { int compute() const override { return 2; } };
// ...

int main() {
    auto container = ContiguousPolyContainer<Base>{ };
    container.push_back(D1{ });
    container.push_back(D2{ });
    // container.push_back(std::make_unique<D2>()); for the non-contiguous PolyContainer
    // ...

    container.for_each([](const Base &item) {
        std::cout << item.compute() << std::endl;
    });
}
```

## Running the tests
unit tests:
```
cd test
./download-dependencies.sh
make
./test
```

execution time benchmark:
```
cd benchmark/benchmark-time
./download-dependencies.sh
make
./benchmark
```

branch and cache misses benchmark: (Note: depends on the Linux-specific tool `perf stat`)
```
cd benchmark/benchmark-branch-cache
make
./run-benchmark.sh
```
