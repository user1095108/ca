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

  s.split(
    [&](auto const b, auto e) noexcept
    {
      auto const l(d.last());

      auto const n0(std::min(std::size_t(e - b), d.capacity() - d.size()));
      auto const n1(std::size_t(&d.data()[d.array_size() - 1] - l) + 1);

      if (r += n0, d.resize(d.size() + n0); n1 < n0)
      {
        std::copy(e = b + n1, b + n0, d.data());
      }
      else
      {
        e = b + n0;
      }

      std::copy(b, e, l);
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
