#include <iostream>

#include "array.hpp"

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
    ca.begin(),
    ca.end(),
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

  return 0;
}
