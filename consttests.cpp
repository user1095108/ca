#include <iostream>
#include "array.hpp"

template <auto C> static constexpr auto coeff() noexcept { return C; }

template <auto C>
void print()
{
  std::cout << C << std::endl;
}

template <std::size_t I>
consteval auto get() noexcept
{
  ca::array<int, 10> const ca{1, 2, 3};

  return ca[I];
}

int main()
{
  print<ca::array<int, 10>{1, 2, 3}.front()>();
  print<coeff<get<2>()>()>();

  return 0;
}
