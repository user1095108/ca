#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>

#include "array.hpp"

void randomize(auto& ...d) noexcept
{
  (
    [&]() noexcept
    {
      d.resize(rand() % d.capacity() + 1);
      d.clear();
    }(),
    ...
  );
}

int main()
{
  std::srand(std::random_device{}());

  ca::array<int, 100> s, d;

  randomize(s, d);

  s.resize(s.capacity());
  std::iota(s.begin(), s.end(), 0);

  int tmp[s.capacity()];
  s.copy(tmp);

  d.append(tmp, std::size(tmp));
  assert(s == d);

  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
