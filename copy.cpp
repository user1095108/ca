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
      auto const sz0(std::size_t(e - b));
      auto const l(d.last());
      auto const sz1(std::size_t(&d.data()[d.array_size() - 1] - l));

      if (d.resize(d.size() + sz0); sz1 < sz0)
      {
        auto const ds(sz0 - sz1);
        std::copy(b, e - ds, l);
        std::copy(e - ds, e, d.data());
      }
      else
      {
        std::copy(b, e, l);
      }
    }
  );

  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
