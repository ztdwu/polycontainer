# PolyContainer [![Build Status](https://travis-ci.org/ztdwu/polycontainer.svg?branch=master)](https://travis-ci.org/ztdwu/polycontainer)
Containers of polymorphic types can often be slow to iterate because the processor has to branch-predict which virtual function implementation to call. As well, polymorphics types are usually stored non-contiguously through pointers, which is severely cache-unfriendly as it hampers the processor's ability to predict and prefetch the next block of data. PolyContainer is a container based on the ideas in an excellent [blog post](http://bannalia.blogspot.ca/2014/05/fast-polymorphic-collections.html) (a must read) by Joaquín M López Muñoz. It stores objects of each derived type contiguously in its own bucket.

In addition to the contiguous polymorphic container (ContinuousPolyContainer), there's also a non-contiguous version (PolyContainer) which, instead of laying out objects next to each other in memory, stores smart pointers to these objects. This allows for easier deletions of individual objects in the container while retaining the benefits of being branch-predictor friendly (up to a certain size, see below), but loses the huge benefit of cache locality.

## Benchmarks (linear iteration):
These microbenchmarks are compiled with Clang 3.6 on Linux 4.3.3 x86_64 (8 X 2400.09 MHz CPU s)

Execution time:
```
|---------------------------------------------------------------------------|
|            |                     Time in ns                               |
| Container  |--------------------------------------------------------------|
|    size    |      std::vector |    PolyContainer | ContiguousPolyContainer|
|------------|--------------------------------------------------------------|
|         16 |              983 |            1,255 |              964       |
|         64 |            2,458 |            2,174 |            1,281       |
|        512 |           14,984 |            9,817 |            3,368       |
|         4k |          113,140 |           46,143 |           15,400       |
|        32k |        1,061,995 |          716,675 |          133,314       |
|       256k |        8,981,431 |       13,444,367 |        1,011,531       |
|      1024k |       38,703,833 |       70,370,333 |        4,152,715       |
|---------------------------------------------------------------------------|

```

Branch and cache misses:
```
|-------------------------------------------------------------------------|
|               |                    branch-misses                        |
|   Container   |---------------------------------------------------------|
|        size   |   std::vector |  PolyContainer | ContiguousPolyContainer|
|---------------|---------------------------------------------------------|
|         100   |        20,443 |         20,624 |           20,891       |
|       1,000   |        22,863 |         21,930 |           21,751       |
|      10,000   |        46,082 |         36,148 |           29,883       |
|     100,000   |       273,946 |        110,618 |          106,999       |
|   1,000,000   |     2,542,425 |        909,713 |          872,826       |
|  10,000,000   |    25,290,224 |      8,581,679 |        8,436,820       |
|-------------------------------------------------------------------------|


|-------------------------------------------------------------------------|
|               |                    cache-misses                         |
|   Container   |---------------------------------------------------------|
|        size   |   std::vector |  PolyContainer | ContiguousPolyContainer|
|--------------------------------------------------------------------------
|         100   |         7,737 |          7,913 |           7,692        |
|       1,000   |         8,665 |          9,965 |           5,844        |
|      10,000   |        10,926 |         11,759 |           6,344        |
|     100,000   |        21,987 |        131,276 |          11,534        |
|   1,000,000   |       227,564 |      2,012,023 |          59,739        |
|  10,000,000   |     2,672,828 |     19,703,591 |         466,345        |
|-------------------------------------------------------------------------|
```

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
