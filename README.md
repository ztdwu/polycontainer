# PolyContainer
Containers of polymorphic types can often be slow to iterate because the processor has to branch-predict which virtual function implementation to call. As well, polymorphics types are usually stored non-contiguously through pointers, which is severely cache-unfriendly as it hampers the processor's ability to predict and prefetch the next block of data. PolyContainer is a container based on the ideas in an excellent [blog post] (http://bannalia.blogspot.ca/2014/05/fast-polymorphic-collections.html) (a must read) by Joaquín M López Muñoz. It stores objects of each derived type contiguously in its own bucket.

In addition to the contiguous polymorphic container (ContinuousPolyContainer), there's also a non-contiguous version (PolyContainer) which, instead of laying out objects next to each other in memory, stores smart pointers to these objects. This allows for easier deletions of individual objects in the container while retaining the benefits of being branch-predictor friendly, but loses the huge benefit of cache locality.

### Benchmarks:
These micro, unscientific benchmarks are compiled with Clang 3.6 on Linux 4.3.3 x86_64
```
Run on (2 X 2893.43 MHz CPU s)
2016-01-14 22:44:21
Benchmark                      Time(ns)    CPU(ns) Iterations
-------------------------------------------------------------
std_vector/32                      2,015       1,738     456523
std_vector/64                      3,356       3,000     210004
std_vector/512                    18,329      18,933      37500
std_vector/4k                    139,957     145,467       5385
std_vector/32k                 1,345,741   1,356,235        553
std_vector/64k                 2,983,769   2,923,516        244
polycontainer_pointers/32          1,875       1,779     466664
polycontainer_pointers/64          2,728       2,446     291668
polycontainer_pointers/512        12,577      12,568      56756
polycontainer_pointers/4k         67,236      67,725      12354
polycontainer_pointers/32k     1,024,591     985,776        656
polycontainer_pointers/64k     2,583,031   2,617,391        284
polycontainer_contiguous/32        1,740       1,693     488356
polycontainer_contiguous/64        1,937       1,798     381819
polycontainer_contiguous/512       4,600       4,251     161536
polycontainer_contiguous/4k       22,150      22,560      34426
polycontainer_contiguous/32k     162,925     151,586       4200
polycontainer_contiguous/64k     341,946     298,823       2019
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
