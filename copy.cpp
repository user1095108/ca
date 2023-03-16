#include <iostream>
#include <numeric>
#include <cassert>

#include "array.hpp"

int main()
{
  ca::array<int, 1024> s, d;

  s.resize(s.capacity());
  std::iota(s.begin(), s.end(), 0);

  ca::split(
    s.begin(),
    s.end(),
    [&](auto b, auto const e) noexcept
    {
      auto const sz0(e - b);
      auto l(d.last());
      auto const sz1(&d.data()[d.array_size() - 1] - l);

      d.resize(d.size() + sz0);
      if (sz1 < sz0)
      {
        auto const lim = b + sz1;
        std::copy(b, lim, l);
        b = lim;
        l = d.data();
      }
      std::copy(b, e, l);
    }
  );
  assert(s == d);

  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
