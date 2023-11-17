#include <iostream>
#include "array.hpp"

template <auto C> static constexpr auto coeff() noexcept { return C; }

template <auto C>
void print()
{
  std::cout << C << std::endl;
}

template <std::size_t I>
constexpr auto get_index()
{
  ca::array<int, 10> const ca{1, 2, 3};

  return ca[I];
}

int main()
{
  print<ca::array<int, 10>{1, 2, 3}.front()>();
  print<coeff<get_index<2>()>()>();

  return 0;
}
