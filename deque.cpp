#include <iostream>

#include "deque.hpp"

int main()
{
  ca::deque<int, 5> dq;

  dq.push_back(1);
  dq.push_back(2);
  dq.push_back(3);

  for (bool r(true); r;)
  {
    if (int v; r = dq.pop_back(v))
    {
      std::cout << v << std::endl;
    }
  }

  return 0;
}
