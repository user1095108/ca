# ca
This is a c++ implementation of an array deque (circular buffer).

Every iterator is invalidated after insertion or erasure, including `end()`. Returned iterators are always valid.

# build instructions
    g++ -std=c++20 -Ofast -D_GLIBCXX_USE_TBB_PAR_BACKEND=0 array.cpp -o a

# resources
* [Open Data Structures](https://opendatastructures.org/)
