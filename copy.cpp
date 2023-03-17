#include <iostream>
#include <numeric>

#include "array.hpp"

template <
  typename T1, std::size_t S1, enum ca::Method M1,
  typename T2, std::size_t S2, enum ca::Method M2
>
auto copy(ca::array<T1, S1, M1> const& s, ca::array<T2, S2, M2>& d) noexcept
{
  std::size_t r{};

  ca::split(
    s.begin(),
    s.end(),
    [&](auto const b, decltype(b) e) noexcept
    {
      auto const sz0(std::min(std::size_t(e - b), d.capacity() - d.size()));
      auto const l(d.last());
      auto const sz1(std::size_t(&d.data()[d.array_size() - 1] - l) + 1);

      if (d.resize(d.size() + sz0); sz1 < sz0)
      {
        std::copy(b + sz1, b + sz0, d.data());
      }

      std::copy(b, b + std::min(sz0, sz1), l);

      r += sz0;
    }
  );

  return r;
}

int main()
{
  ca::array<int, 1024> s, d;

  s.resize(s.capacity());
  std::iota(s.begin(), s.end(), 0);

  std::cout << copy(s, d) << std::endl;

  std::copy(d.crbegin(), d.crend(), std::ostream_iterator<int>(std::cout, "\n"));

  return 0;
}
