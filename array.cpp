#include <iostream>

//#define NO_CA_ARRAY_CONTRIB // uncommenting this line, will slow the copying into la2
#include "array.hpp"
#include <numeric>
#include <chrono>
#include <cassert>

int main()
{
  ca::array<int, 3> ca;

  ca.push_front(1);
  ca.push_back(2);
  ca.push_back(3);
  ca.push_front(4);

  std::sort(ca.begin(), ca.end());

  std::cout << "size: " << ca.size() << std::endl;
  std::cout << "full: " << ca.full() << std::endl;

  std::copy(ca.cbegin(), ca.cend(), std::ostream_iterator<int>(std::cout, "\n"));

  ca.insert(ca.cend(), 5);
  ca.push_back(10);

  std::cout << "size: " << ca.size() << std::endl;
  std::cout << "full: " << ca.full() << std::endl;

  ca::split(
    ca.cbegin(),
    ca.cend(),
    [](auto const b, auto const e)
    {
      std::copy(b, e, std::ostream_iterator<int>(std::cout, "\n"));
    }
  );

  ca.erase(std::next(ca.cbegin()));

  std::cout << "size: " << ca.size() << std::endl;
  std::cout << "full: " << ca.full() << std::endl;

  std::for_each(
    ca.cbegin(),
    ca.cend(),
    [](auto&& v) noexcept
    {
      std::cout << v << std::endl;
    }
  );

  std::cout << std::distance(ca.cbegin(), ca.cend()) << " : " << ca.size() << " " << ca[0] << std::endl;

  {
    ca::array<int, 500000> la; // large array
    std::generate_n(std::back_inserter(la),la.capacity(),[i=1]() mutable{ return i++; });

    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    ca::array<int, la.capacity()> la2(la);
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    assert(la == la2);
  }

  return 0;
}
