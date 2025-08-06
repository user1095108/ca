#include <algorithm>
#include <functional>
#include <iostream>
#include "array.hpp"

template <auto C> static constexpr auto coeff() noexcept { return C; }

template <auto C>
void print()
{
  std::cout << C << std::endl;
}

consteval auto max_element() noexcept
{
  dq::array<int, 10, dq::NEW> ca{1, 2, 3};
  ca.push_back(4, 5, 6, 111);
  std::sort(ca.begin(), ca.end(), std::greater<>());

  return ca.front();
}

template <auto C> void test() { }

int main()
{
  print<dq::array<int, 10>{1, 2, 3}.front()>();
  print<coeff<max_element()>()>();

  static constexpr dq::array<int, 10> a{1, 2, 3};
  test<a>();
  return 0;
}
