#include <iostream>
#include <numeric>

#include "array.hpp"

int main()
{
  ca::array<int, 1024> s, d;

  s.resize(s.capacity());
  std::iota(s.begin(), s.end(), 0);

  ca::split(
    s.begin(),
    s.end(),
    [&](auto const b, decltype(b) e) noexcept
    {
      std::size_t const sz0(e - b);
      auto const l(d.last());
      auto const sz1(std::size_t(&d.data()[d.array_size() - 1] - l) + 1);
      std::size_t ds;

      if (d.resize(d.size() + sz0); sz1 < sz0)
      {
        ds = sz0 - sz1;
        std::copy(e - ds, e, d.data());
      }
      else
      {
        ds = {};
      }

      std::copy(b, e - ds, l);
    }
  );

  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
