[![Build Status](https://travis-ci.org/ztdwu/polycontainer.svg?branch=master)](https://travis-ci.org/ztdwu/polycontainer)
[![codecov.io](https://codecov.io/github/ztdwu/polycontainer/coverage.svg?branch=master)](https://codecov.io/github/ztdwu/polycontainer?branch=master)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE?raw=true) 
# PolyContainer 
This library offers two types of containers: `ContinuousPolyContainer` and `PolyContainer`.
- `ContinuousPolyContainer` lays out each object contiguously in memory, offering the best cache locality as well as branch predictability. 
- `PolyContainer` holds smart pointers to each element in the container, which loses the huge benefit of cache locality (and can be quite slow for large container sizes, see benchmarks below). However, this allows for easier deletions of individual objects in the container while retaining the benefits of being branch-predictor friendly.

This library is an implementation of the idea presented in the excellent [blog post](http://bannalia.blogspot.ca/2014/05/fast-polymorphic-collections.html) (a must read) by Joaquín M López Muñoz

---

## Benchmarks (linear iteration)
These microbenchmarks are compiled with Clang 3.6 on Linux 4.3.3 x86_64 (8 X 2400.09 MHz CPU s)

![Benchmarks Graph](/benchmark/benchmarks.png?raw=true)

`ContiguousPolyContainer` appears to be easily the fastest, and seems scale linearly with the number of elements.

The non-contiguous `PolyContainer` is a significantly faster than `std::vector` up until around 32k elements, after which its performance starts to degrade severely due to a high number of cache misses (even more than `std::vector`!), so use with caution.

---

## Usage

Constructs an empty container
```c++
auto container1 = ContiguousPolyContainer<Base>{ };
auto container2 = PolyContainer<Base>{ };
```

Inserts and returns a reference to the inserted value
```c++
auto &inserted = container.push_back(item);              // ContiguousPolyContainer, insert by lvalue ref
auto &inserted = container.push_back(std::move(item));   // ContiguousPolyContainer, insert by rvalue ref
auto &inserted = container.push_back(DerivedType{ });    // ContiguousPolyContainer, insert by rvalue ref

auto &inserted = container.push_back(std::move(my_unique_ptr));        // PolyContainer, insert by rvalue ref
auto &inserted = container.push_back(std::make_unique<DerivedType>()); // PolyContainer, insert by rvalue ref
```

Calls the lambda for each item in the container
```c++
container.for_each([](auto &item) { });
```

Returns a reference to the underlying vector that holds all items of DerivedType
```c++
auto &segment = container.get_segment<DerivedType>();
// Return type is std::vector<DerivedType> for ContiguousPolyContainer
// and std::vector<std::unique_ptr<Base>> for PolyContainer
```

Returns the total number of items in the container
```c++
const auto len = container.len();
```

Removes all items
```c++
container.clear();
```

TODO:
```c++
container.begin(); // begin, cbegin, rbegin, crbegin
container.end();   // end, cend, rend, crend
container.erase(iter);
```

---

## Limitations
There is an unfortunate (but necessary) limitation in the `ContiguousPolyContainer` class (which unfortunately isn't mentioned in the linked blog post above, even though it shares the same limitation). Consider the following function signiture for `ContiguousPolyContainer::push_back`:
```c++
template <typename D>
D& push_back(D &&d);
```
And if we were to use `push_back` like this:
```c++
auto item = Derived{ };
Base &ref = item;
container.push_back(ref);
```
In this case, C++'s template type deduction will resolve the template argument `typename D` to `Base` rather than `Derived` (ref removed for simplicity). This, in turn, will insert an item of type `Derived` into a vector of type `std::vector<Base>` (assuming Base is non-pure-virtual), which will cause slicing. As a result, all items must be referenced by their dynamic type rather than base type when being inserted into `ContiguousPolyContainer`, otherwise the exception `BadDerivedTypeException` will be thrown.

---

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
