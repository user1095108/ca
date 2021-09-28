# ca
This is c++ circular array implementation that I use for queing requests (i.e. for quick pushing and popping). It also supports `erase()`, but not `resize()`, as that would defeat the purpose of the implementation.

# build instructions
    g++ -std=c++20 -Ofast ca.cpp -o ca
