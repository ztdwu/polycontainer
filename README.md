# PolyContainer [![Build Status](https://travis-ci.org/ztdwu/polycontainer.png?branch=master)](https://travis-ci.org/ztdwu/polycontainer)
Containers of polymorphic types can often be slow to iterate because the processor has to branch-predict which virtual function implementation to call. As well, polymorphics types are usually stored non-contiguously through pointers, which is severely cache-unfriendly as it hampers the processor's ability to predict and prefetch the next block of data. PolyContainer is a container based on the ideas in an excellent [blog post] (http://bannalia.blogspot.ca/2014/05/fast-polymorphic-collections.html) (a must read) by Joaquín M López Muñoz. It stores objects of each derived type contiguously in its own bucket.

In addition to the contiguous polymorphic container (ContinuousPolyContainer), there's also a non-contiguous version (PolyContainer) which, instead of laying out objects next to each other in memory, stores smart pointers to these objects. This allows for easier deletions of individual objects in the container while retaining the benefits of being branch-predictor friendly (up to a certain size, see below), but loses the huge benefit of cache locality.

### Benchmarks (linear iteration):
These micro, unscientific benchmarks are compiled with Clang 3.6 on Linux 4.3.3 x86_64
```
Run on (2 X 2893.43 MHz CPU s)
2016-01-14 22:44:21
Benchmark                              Time(ns)      CPU(ns)   Iterations
------------------------------------------------------------------------

std_vector/32                             1,939        1,850      333343
std_vector/64                             3,184        3,045      238641
std_vector/512                           17,051       17,415       39622
std_vector/4k                           130,212      128,483        5526
std_vector/32k                        1,241,313    1,299,857         677
std_vector/256k                      11,101,877   11,129,129          62
std_vector/1024k                     43,868,154   43,541,500          16

polycontainer_pointers/32                 1,883        1,731      446810
polycontainer_pointers/64                 2,665        2,179      344261
polycontainer_pointers/512               12,354       12,934       72417
polycontainer_pointers/4k                69,895       69,079       13125
polycontainer_pointers/32k              933,672      942,864         700
polycontainer_pointers/256k          16,027,895   16,287,864          44
polycontainer_pointers/1024k         75,466,728   75,333,300          10

polycontainer_contiguous/32               1,710        1,489      355924
polycontainer_contiguous/64               1,947        1,750      396228
polycontainer_contiguous/512              4,406        4,265      157893
polycontainer_contiguous/4k              21,310       21,809       35000
polycontainer_contiguous/32k            155,236      157,670        4038
polycontainer_contiguous/256k         1,352,541    1,249,984         568
polycontainer_contiguous/1024k        5,031,790    5,080,621         124
```
ContiguousPolyContainer is by far the fastest, and appears to scale linearly with the number of elements. The non-contiguous PolyContainer is a significantly faster than std_vector up until around 32k elements, after which its performance starts to degrade severely.


### Mini Documentation:
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

### Example:
```c++
#include "polycontainer.hpp"
#include <iostream>
using polycontainer::PolyContainer;

struct Base {
    virtual ~Base() { }
    virtual int compute() const = 0;
};

struct Derived1 : Base { int compute() const override { return 1; } };
struct Derived2 : Base { int compute() const override { return 2; } };
// ...

int main() {
    auto container = PolyContainer<Base>{ };
    container.push_back(std::make_unique<Derived1>());
    container.push_back(std::make_unique<Derived2>());
    // ...
    // container.push_back(Derived2{ }); for ContiguousPolyContainer

    container.for_each([](const Base &item) {
        std::cout << item.compute() << std::endl;
    });
}
```

### Tests
To run the benchmark or the unit tests, `cd` into the `benchmark` or `test` directory and run:
```bash
./download-dependencies.sh
make
./test ## or ./benchmark depending on the currect directory
```
