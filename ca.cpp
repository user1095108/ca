#include <iostream>

#include "ca.hpp"

int main()
{
  ca::circular_array<int, 3> ca;

  ca.push_front(1); // 1 3 2
  ca.push_front(2);
  ca.push_front(3);
  ca.push_front(4);
  ca.push_front(5);
  ca.push_front(10);

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

  return 0;
}
