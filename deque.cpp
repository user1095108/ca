#include <iostream>

#include "deque.hpp"

int main()
{
  ca::deque<int, 5> dq;

  dq.push_back(1);
  dq.push_back(2);
  dq.push_back(3);

  while (dq.size())
  {
    if (int v; dq.pop_back(v))
    {
      std::cout << v << std::endl;
    }
  }

  return 0;
}
