#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>

#include "array.hpp"

void randomize(auto& ...a) noexcept
{
  (
    [&]() noexcept
    {
      a.resize(std::rand() % a.capacity() + 1);
      a.clear();
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
  ca::copy(s, tmp);

  d.append(tmp, std::size(tmp));
  assert(s == d);

  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
