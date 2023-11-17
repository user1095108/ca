#include <iostream>
#include "array.hpp"

template <auto C>
void print()
{
  std::cout << C << std::endl;
}

int main()
{
  print<ca::array<int, 10>{1, 2, 3}.front()>();

  return 0;
}
