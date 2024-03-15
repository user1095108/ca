#include <cassert>
#include <iostream>
#include <numeric>

#include "array.hpp"

int main()
{
  ca::array<int, 100> s, d;

  s.resize(s.capacity());
  std::iota(s.begin(), s.end(), 0);

  d.append(s.data(), s.size());
  assert(s == d);
  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  s.copy(d.data());
  assert(s == d);
  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
