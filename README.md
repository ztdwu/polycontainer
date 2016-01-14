# PolyContainer
Containers of polymorphic types can sometimes be slow to iterate because the processor has to branch-predict which virtual function implementation to call. The PolyContainer is a container that can hold polymorphic objects in the same inheritance hierarchy, and place objects of the same derived type into its own buckets for better branch-prediction during iterations. 

## Benchmarks:
These micro, unscientific benchmarks are compiled with Clang 3.6 on Linux 4.3.3 x86_64
```
Run on (2 X 2893.43 MHz CPU s)
2016-01-08 21:04:24
Benchmark                     Time(ns)    CPU(ns) Iterations
------------------------------------------------------------
benchmark_std_vector/32           1809       1752     437505                                 
benchmark_std_vector/64           3172       3077     230771                                 
benchmark_std_vector/512         17424      17219      43751                                 
benchmark_std_vector/4k         144443     149333       7500                                 
benchmark_std_vector/8k         337965     322110       2763                                 
benchmark_polycontainer/32        1817       1658     456517                                 
benchmark_polycontainer/64        2667       2380     368431                                 
benchmark_polycontainer/512      12331      11809      43752                                 
benchmark_polycontainer/4k       64485      66380      12353                                 
benchmark_polycontainer/8k      223341     224912       2727  
```

## Example:
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
    container.for_each([](const Base &item) {
        std::cout << item.compute() << std::endl;
    });
}
```

Thanks to Joaquín M López Muñoz who wrote the excellent [blog post] (http://bannalia.blogspot.ca/2014/05/fast-polymorphic-collections.html)(a must read) that inspired this. One major difference, however, between PolyContainer and Joaquín's library is that PolyContainer stores each element in its own heap location. This will of course throw off the prefetcher and cause a lot of cache misses, but the upside is that it allows the user to delete individual objects any time, whereas if objects are stored contiguously, then it is much harder to release the memory for any individial object (although iterations will also be much faster)
