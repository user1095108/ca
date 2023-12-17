# ca
This is a c++ implementation of an array deque (circular buffer).

Every iterator is invalidated after insertion or erasure, including `end()`. Returned iterators are always valid.

# build instructions
    g++ -std=c++20 -Ofast ca.cpp -o ca

# resources
* [Open Data Structures](https://opendatastructures.org/)
