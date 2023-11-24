#include <cassert>
#include <iostream>
#include <memory>
#include <random>

#include "array.hpp" // Replace with the actual container header

// Include your testing framework of choice (e.g., Google Test or Catch2)

// Function to test various operations of your random-access container
void test1() {
  {
  // Create an instance of your container
  ca::array<int, 20> dq(ca::multi, 1, 2, 3);

  // Test the size of the container
  assert(dq.size() == 3);

  // Test accessing elements by index
  assert(dq[0] == 1);
  assert(dq[1] == 2);
  assert(dq[2] == 3);

  // Test modifying elements
  dq[1] = 42;
  assert(dq[1] == 42);

  // Test removing elements
  dq.pop_back();
  assert(dq.size() == 2);

  // Add more test cases for your container's methods (e.g., clear, resize, etc.)

  // Test iterator functionality
  auto it = dq.begin();
  assert(*it == 1);
  ++it;
  assert(*it == 42);
  ++it;
  assert(it == dq.end());

  // Test edge cases
  dq.clear();
  assert(dq.empty());
  assert(dq.size() == 0);

  // Additional test cases:

  // Test inserting elements at a specific position
  dq.push_back(4);
  dq.insert(dq.begin() + 1, 5);
  assert(dq.size() == 2);
  assert(dq[0] == 4);
  assert(dq[1] == 5);

  // Test erasing elements at a specific position
  dq.erase(dq.begin());
  assert(dq.size() == 1);
  assert(dq[0] == 5);

  // Test resizing the container
  dq.resize(5);
  assert(dq.size() == 5);

  // Test swapping containers
  decltype(dq) otherContainer;
  otherContainer.push_back(6);
  otherContainer.push_back(7);
  dq.swap(otherContainer);
  assert(dq.size() == 2);
  assert(otherContainer.size() == 5);

  // Test copying a container
  decltype(dq) copiedContainer = dq;
  assert(copiedContainer.size() == 2);
  assert(copiedContainer[0] == 6);

  // Test range-based for loop
  int sum = 0;
  for (const auto& item : dq) {
      sum += item;
  }

  assert(sum == 13);

  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Accessing elements using iterators
  auto it = dq.begin();
  assert(*it == 1);

  // Modifying elements
  *it = 10;
  assert(*it == 10);

  // Inserting elements at the front and back
  dq.push_front(0);
  dq.push_back(6);
  assert(dq.front() == 0);
  assert(dq.back() == 6);

  // Erasing elements
  dq.pop_front();
  dq.pop_back();
  assert(dq.front() == 10);
  assert(dq.back() == 5);

  // Inserting elements in the middle
  it = dq.insert(std::next(dq.begin(), 2), 99);
  assert(*it == 99);
  assert(dq[2] == 99);

  // Erasing elements from the middle
  it = dq.erase(std::next(dq.begin(), 3));
  assert(*it == 4);
  assert(dq[3] == 4);
  }

  {
  // Create a deque
  ca::array<int, 10> d;

  // Insert elements at the front and back of the deque
  d.push_front(1);
  d.push_back(2);
  assert(d.front() == 1 && d.back() == 2);

  // Delete elements from the front and back of the deque
  d.pop_front();
  d.pop_back();
  assert(d.empty());

  // Insert elements at the front and back of the deque
  d.push_front(3);
  d.push_back(4);
  assert(d.front() == 3 && d.back() == 4);

  // Check the size of the deque
  assert(d.size() == 2);

  // Resize the deque
  d.resize(1);
  assert(d.size() == 1 && d.front() == 3);

  // Clear the deque
  d.clear();
  assert(d.empty());

  // Insert elements and swap the contents of two deques
  d.push_back(5);
  ca::array<int, 10> d2;
  d2.push_back(6);
  d.swap(d2);
  assert(d.front() == 6 && d2.front() == 5);
  }

  {
  ca::array<int, 20> dq;

  // Pushing elements to the back
  for (int i = 1; i <= 5; ++i) {
      dq.push_back(i);
  }

  // Pushing elements to the front
  for (int i = 10; i <= 15; ++i) {
      dq.push_front(i);
  }

  // Asserting the elements
  assert(dq.size() == 11);
  assert(dq[0] == 15);
  assert(dq[1] == 14);
  assert(dq[2] == 13); // --
  assert(dq[3] == 12);
  assert(dq[4] == 11);
  assert(dq[5] == 10);
  assert(dq[6] == 1);
  assert(dq[7] == 2);
  assert(dq[8] == 3); // -- 
  assert(dq[9] == 4);
  assert(dq[10] == 5);

  // Erasing elements
  dq.erase(dq.begin() + 2);
  dq.erase(dq.end() - 3);

  // Asserting the modified deque
  assert(dq.size() == 9);
  assert(dq[0] == 15);
  assert(dq[1] == 14);
  assert(dq[2] == 12);
  assert(dq[3] == 11);
  assert(dq[4] == 10);
  assert(dq[5] == 1);
  assert(dq[6] == 2);
  assert(dq[7] == 4);
  assert(dq[8] == 5);

  // Inserting elements
  dq.insert(dq.begin() + 3, 100);
  dq.insert(dq.end() - 1, 200);

  // Asserting the final deque
  assert(dq.size() == 11);
  assert(dq[0] == 15);
  assert(dq[1] == 14);
  assert(dq[2] == 12);
  assert(dq[3] == 100);
  assert(dq[4] == 11);
  assert(dq[5] == 10);
  assert(dq[6] == 1);
  assert(dq[7] == 2);
  assert(dq[8] == 4);
  assert(dq[9] == 200);
  assert(dq[10] == 5);
  }
}

void test2() {
  {
  ca::array<int, 20> dq1; // default constructor
  assert(dq1.empty()); // should be empty

  ca::array<int, 20> dq2 = {1, 2, 3, 4, 5}; // initializer list constructor
  assert(!dq2.empty()); // should not be empty
  assert(dq2.size() == 5); // should contain 5 elements

  ca::array<int, 20> dq3(dq2); // copy constructor
  assert(dq3 == dq2); // should be equal to dq2

  ca::array<int, 20> dq4(std::move(dq2)); // move constructor
  assert(dq4 == dq3); // should be equal to dq3
  assert(dq2.empty()); // dq2 should now be empty

  // Test assignment operators
  dq1 = dq3; // copy assignment operator
  assert(dq1 == dq3); // should be equal to dq3

  dq2 = std::move(dq3); // move assignment operator
  assert(dq2 == dq4); // should be equal to dq4
  assert(dq3.empty()); // dq3 should now be empty
  }

  {
  ca::array<int, 20> dq = {1, 2, 3, 4, 5};

  // Test element access
  assert(dq.at(0) == 1);
  assert(dq[1] == 2);
  assert(dq.front() == 1);
  assert(dq.back() == 5);

  // Test capacity
  assert(!dq.empty());
  assert(dq.size() == 5);
  }

  {
  ca::array<int, 20, ca::NEW> dq;

  // Test push_back and push_front
  dq.push_back(2);
  dq.push_front(1);
  dq.push_back(3);
  assert(dq.front() == 1);
  assert(dq.back() == 3);

  // Test emplace_back and emplace_front
  dq.emplace_back(4);
  dq.emplace_front(0);
  assert(dq.front() == 0);
  assert(dq.back() == 4);

  // Test pop_back and pop_front
  dq.pop_back();
  assert(dq.back() == 3);
  dq.pop_front();
  assert(dq.front() == 1);

  // Test resize
  dq.resize(5);
  assert(dq.size() == 5);

  // Test swap
  ca::array<int, 20, ca::NEW> dq2 = {6, 7, 8, 9, 10};
  dq.swap(dq2);
  assert(dq.front() == 6);
  }

  {
  ca::array<int, 20> dq = {1, 2, 3, 4, 5};

  // Test iterators
  assert(*dq.begin() == 1);
  assert(*(dq.end() - 1) == 5);
  assert(*dq.rbegin() == 5);
  assert(*(dq.rend() - 1) == 1);

  const ca::array<int, 20> cdq = dq;
  assert(*cdq.cbegin() == 1);
  assert(*(cdq.cend() - 1) == 5);
  assert(*cdq.crbegin() == 5);
  assert(*(cdq.crend() - 1) == 1);
  }

  {
  ca::array<int, 20> dq1 = {1, 2, 3, 4, 5};
  ca::array<int, 20> dq2 = {1, 2, 3, 4, 5};
  ca::array<int, 20> dq3 = {1, 2, 3, 4, 6};

  // Test operators
  assert(dq1 == dq2);
  assert(dq1 != dq3);
  assert(dq1 < dq3);
  assert(dq1 <= dq2);
  assert(dq3 > dq1);
  assert(dq2 >= dq1);
  }

  {
  ca::array<int, 20> dq = {1, 2, 3, 4, 5};

  // Test insert
  dq.insert(dq.begin() + 2, 6);
  assert((dq == ca::array<int, 20>{1, 2, 6, 3, 4, 5}));

  // Test emplace
  dq.emplace(dq.begin() + 3, 7);
  assert((dq == ca::array<int, 20>{1, 2, 6, 7, 3, 4, 5}));

  // Test erase
  dq.erase(dq.begin() + 2, dq.begin() + 4);
  assert((dq == ca::array<int, 20>{1, 2, 3, 4, 5}));

  // Test clear
  dq.clear();
  assert(dq.empty());
  }

  {
  ca::array<int, 20> dq;

  // Test push_front and push_back with an empty deque
  dq.push_front(1);
  assert(dq.front() == 1);
  dq.push_back(2);
  assert(dq.back() == 2);

  // Test pop_front and pop_back
  dq.pop_front();
  assert(dq.front() == 2);
  dq.pop_back();
  assert(dq.empty());
  }

  {
  // Test constructors
  ca::array<std::string, 10> dq1; // default constructor
  assert(dq1.empty()); // should be empty

  ca::array<std::string, 10> dq2 = {"one", "two", "three"}; // initializer list constructor
  assert(!dq2.empty()); // should not be empty
  assert(dq2.size() == 3); // should contain 3 elements

  ca::array<std::string, 10> dq3(dq2); // copy constructor
  assert(dq3 == dq2); // should be equal to dq2

  ca::array<std::string, 10> dq4(std::move(dq2)); // move constructor
  assert(dq4 == dq3); // should be equal to dq3
  assert(dq2.empty()); // dq2 should now be empty

  // Test assignment operators
  dq1 = dq3; // copy assignment operator
  assert(dq1 == dq3); // should be equal to dq3

  dq2 = std::move(dq3); // move assignment operator
  assert(dq2 == dq4); // should be equal to dq4
  assert(dq3.empty()); // dq3 should now be empty
  }

  {
  ca::array<int, 10> dq1 = {1, 2, 3};
  ca::array<int, 10> dq2 = {4, 5, 6, 7};

  // Test operator=
  dq1 = dq2;
  assert(dq1 == dq2); // dq1 should now be equal to dq2
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test operator[]
  assert(dq[0] == 1);
  assert(dq[2] == 3);
  assert(dq[4] == 5);
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test find
  auto it = std::find(dq.begin(), dq.end(), 3);
  assert(it != dq.end() && *it == 3);

  it = std::find(dq.begin(), dq.end(), 6);
  assert(it == dq.end());
  }

  {
  ca::array<int, 10> dq1 = {1, 2, 3};
  ca::array<int, 10> dq2 = {4, 5, 6, 7};

  // Test swap
  dq1.swap(dq2);
  assert((dq1 == ca::array<int, 10>{4, 5, 6, 7}));
  assert((dq2 == ca::array<int, 10>{1, 2, 3}));
  }

  {
  ca::array<int, 10> dq1 = {1, 2, 3};
  ca::array<int, 10> dq2 = {1, 2, 3};
  ca::array<int, 10> dq3 = {1, 2, 3, 4};

  // Test comparison operators
  assert(dq1 == dq2);
  assert(dq1 != dq3);
  assert(dq1 < dq3);
  assert(dq1 <= dq2);
  assert(dq3 > dq1);
  assert(dq2 >= dq1);
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test resize
  dq.resize(3);
  assert(dq.size() == 3);
  assert((dq == ca::array<int, 10>{1, 2, 3}));

  dq.resize(5);
  assert(dq.size() == 5);
  }

  {
  ca::array<int, 10> dq;

  // Test emplace_front and emplace_back
  dq.emplace_front(1);
  assert(dq.front() == 1);
  dq.emplace_back(2);
  assert(dq.back() == 2);
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test insert
  dq.insert(dq.begin() + 2, 6);
  assert((dq == ca::array<int, 10>{1, 2, 6, 3, 4, 5}));
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test emplace
  dq.emplace(dq.begin() + 2, 6);
  assert((dq == ca::array<int, 10>{1, 2, 6, 3, 4, 5}));
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test erase
  dq.erase(dq.begin() + 2, dq.begin() + 4);
  assert((dq == ca::array<int, 10>{1, 2, 5}));
  }

  {
  ca::array<int, 10> dq = {1, 2, 4, 5};
  ca::array<int, 10> to_insert = {3};

  // Test insert
  dq.insert(dq.begin() + 2, to_insert.begin(), to_insert.end());
  assert((dq == ca::array<int, 10>{1, 2, 3, 4, 5}));
  }

  {
  ca::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // Test erase
  dq.erase(dq.begin());
  assert((dq == ca::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9}));

  dq.erase(dq.begin() + 2, dq.begin() + 5);
  assert((dq == ca::array<int, 10>{1, 2, 6, 7, 8, 9}));

  //
  for (auto it = dq.begin(); it != dq.end();) {
    if (*it % 2 == 0)
        it = dq.erase(it);
    else
        ++it;
  }

  assert((dq == ca::array<int, 10>{1, 7, 9}));

  // Erase all elements
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());
  }

  {
  ca::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // Test erase
  dq.erase(dq.begin() + 3);
  assert((dq == ca::array<int, 10>{0, 1, 2, 4, 5, 6, 7, 8, 9}));

  dq.erase(dq.end() - 1);
  assert((dq == ca::array<int, 10>{0, 1, 2, 4, 5, 6, 7, 8}));
  }

  {
  ca::array<int, 10> d1, d2;

  // Insert elements into the first deque
  for (int i = 1; i <= 5; ++i) {
      d1.push_back(i);
  }

  // Insert elements into the second deque
  for (int i = 6; i <= 10; ++i) {
      d2.push_back(i);
  }

  // Swap the contents of the two deques
  d1.swap(d2);

  // Check if the elements have been correctly swapped
  for (int i = 1; i <= 5; ++i) {
      assert(d2[i-1] == i);
  }

  for (int i = 6; i <= 10; ++i) {
      assert(d1[i-6] == i);
  }
  }

  {
  // Create a deque with a maximum size of 5
  ca::array<int, 5> stack(ca::multi, 1, 2, 3, 4, 5);

  // Check if the stack is full
  assert(stack.size() == 5);

  // Try to push another element onto the stack
  stack.push_back(6);

  // Check if the oldest element has been removed
  assert(stack.size() == 5);
  assert(stack.front() == 2);

  // Pop elements from the stack
  stack.pop_front();
  stack.pop_front();
  assert(stack.front() == 4);

  // Clear the stack
  stack.clear();
  assert(stack.empty());
  }

  {
  ca::array<int, 10> dq = {1, 2, 3, 4, 5};

  assert(dq.size() == 5);

  dq.pop_front();
  dq.pop_back();

  assert(dq.size() == 3);
  assert(dq.front() == 2);
  assert(dq.back() == 4);

  dq.push_front(1);
  dq.push_back(5);

  assert(dq.size() == 5);
  assert(dq.front() == 1);
  assert(dq.back() == 5);

  dq.clear();

  assert(dq.empty());
  }

  {
  ca::array<char, 10> palindrome(ca::from_range, std::string_view("racecar"));

  while(palindrome.size() > 1) {
    assert(palindrome.front() == palindrome.back());
    palindrome.pop_front();
    palindrome.pop_back();
  }

  assert(1 == palindrome.size());
  }

  {
  ca::array<std::string, 52> deck;

  // Fill the deck with cards
  std::string suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
  std::string ranks[13] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};

  for(auto &suit : suits) {
      for(auto &rank : ranks) {
          deck.push_back(rank + " of " + suit);
      }
  }

  // The deck should have 52 cards
  assert(deck.size() == 52);

  // Draw the top card
  std::string topCard = deck.front();
  deck.pop_front();
  assert(deck.size() == 51);

  // Draw the bottom card
  std::string bottomCard = deck.back();
  deck.pop_back();
  assert(deck.size() == 50);

  // The top and bottom cards should not be the same
  assert(topCard != bottomCard);
  }

  {
  ca::array<int, 10> deque;

  // Push elements into the deque
  deque.push_back(1);
  deque.push_front(2);
  deque.push_back(3);

  // Use assert to check for results
  assert(deque.size() == 3);
  assert(deque[0] == 2);
  assert(deque[1] == 1);
  assert(deque[2] == 3);

  // Erase an element
  deque.erase(deque.begin() + 1);

  // Use assert to check for results again
  assert(deque.size() == 2);
  assert(deque[0] == 2);
  assert(deque[1] == 3);
  }

  {
  ca::array<std::string, 10> line;

  // People join the line
  line.push_back("Alice");
  line.push_back("Bob");
  line.push_back("Charlie");

  // Check the first person in line
  assert(line.front() == "Alice");

  // A VIP joins the line at the front
  line.push_front("Dave");

  // Check the first person in line now
  assert(line.front() == "Dave");

  // The concert starts, people start buying tickets and leaving the line from the front
  line.pop_front();
  assert(line.front() == "Alice");

  // Another VIP joins the line at the front
  line.push_front("Eve");

  // Check the first person in line now
  assert(line.front() == "Eve");

  // The concert starts, people start buying tickets and leaving the line from the front
  while(!line.empty()) {
      line.pop_front();
  }

  // Check that the line is empty
  assert(line.empty());
  }

  {
  ca::array<std::string, 10> magicBox;

  // Put an item from the front
  magicBox.push_front("Gold");
  assert(magicBox.back() == "Gold");

  // Put an item from the back
  magicBox.push_back("Diamond");
  assert(magicBox.back() == "Diamond");

  // The box should now contain two items
  assert(magicBox.size() == 2);

  // Remove the item from the front
  magicBox.pop_front();
  assert(magicBox.front() == "Diamond");

  // Remove the item from the back
  magicBox.pop_back();
  assert(magicBox.empty());
  }

  {
  ca::array<int, 10> d;

  // Fill the deque with numbers from 0 to 9
  for(int i = 0; i < 10; ++i) d.push_back(i);

  // Get an iterator to the first element
  auto it = d.begin();

  // Advance the iterator by 5 positions
  std::advance(it, 5);
  assert(*it == 5);

  // Advance the iterator by 2 positions
  std::advance(it, 2);
  assert(*it == 7);

  // Move the iterator back by 3 positions
  std::advance(it, -3);
  assert(*it == 4);
  }

  {
  ca::array<int, 10> d = {1, 2, 3, 4, 5};
  d.erase(std::remove_if(d.begin(), d.end(), [](int n){ return n % 2 == 0; }), d.end());
  for (int n : d) assert(n % 2 != 0);
  }

  {
  ca::array<int, 10> d = {1, 2, 3, 4, 5};
  ca::erase_if(d, [](int n){ return n % 2 == 0; });
  for (int n : d) assert(n % 2 != 0);
  }

  {
  ca::array<int, 20> dq{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
  dq.erase(std::unique(dq.begin(), dq.end()), dq.end());
  assert((dq == ca::array<int, 20>{1, 2, 3, 4}));
  }

  {
  ca::array<int, 10> d = {1, 2, 3, 4};
  ca::array<int, 10> l = {-1, -2, -3};
  auto pos = std::next(d.begin(), 2);
  d.insert(pos, l.begin(), l.end());
  assert((d == ca::array<int, 10>{1, 2, -1, -2, -3, 3, 4}));
  }

  {
  ca::array<int, 10> dq1{1, 2, 3, 4, 5};
  ca::array<int, 10> dq2{6, 7, 8, 9, 10};

  // Create a third deque to hold the merged result
  ca::array<int, 10> mdq(10);

  // Merge the two deques
  std::merge(dq1.begin(), dq1.end(), dq2.begin(), dq2.end(), mdq.begin());

  // Expected merged deque
  assert((mdq == ca::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

  // Reverse the merged deque
  std::reverse(mdq.begin(), mdq.end());

  // Expected reversed deque
  assert((mdq == ca::array<int, 10>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
  }

  {
  ca::array<int, 10> originalDeque{1, 2, 3, 4, 5};
  auto deque{originalDeque};

  // Generate a random permutation of the deque's elements.
  std::shuffle(deque.begin(), deque.end(), std::mt19937{std::random_device{}()});

  // Rotate the deque by a random amount greater than zero.
  std::rotate(deque.begin(), deque.begin() + 1 + std::rand() % (deque.size() - 1), deque.end());

  // Split the deque into two halves.
  auto mid = deque.begin() + deque.size() / 2;

  // Reverse the second half of the deque.
  std::reverse(mid, deque.end());
  std::sort(deque.begin(), deque.end());

  // Check if the deque is now in its original order.
  assert(std::equal(deque.begin(), deque.end(), originalDeque.begin()));
  }

  {
  ca::array<int, 10> a(5, 10);

  // Check that the array has the correct size
  assert(a.size() == 5);

  // Check that all elements in the array are 10
  for (const auto& e: a) assert(e == 10);
  }

  {
  ca::array<int, 20> myDeque = {1, 2, 3, 4, 5};

  // Testing insert() at the beginning of the deque
  auto it = myDeque.insert(myDeque.begin(), 0);
  assert(myDeque.size() == 6);
  assert(*it == 0);
  assert(myDeque.front() == 0);

  // Testing insert() in the middle of the deque
  it = myDeque.insert(myDeque.begin() + 3, 6);
  assert(myDeque.size() == 7);
  assert(*it == 6);
  assert(myDeque[3] == 6);

  // Testing insert() at the end of the deque
  it = myDeque.insert(myDeque.end(), 7);
  assert(myDeque.size() == 8);
  assert(*it == 7);
  assert(myDeque.back() == 7);

  // Testing insert() with multiple elements
  ca::array<int, 20> otherDeque = {8, 9, 10};
  myDeque.insert(myDeque.begin() + 2, otherDeque.begin(), otherDeque.end());
  assert(myDeque.size() == 11);
  assert(myDeque[2] == 8);
  assert(myDeque[3] == 9);
  assert(myDeque[4] == 10);
  }

  {
  ca::array<std::unique_ptr<int>, 10> myDeque;
  for (int i = 0; i < 10; i++) {
    myDeque.push_back(std::make_unique<int>(i));
    //myDeque.emplace_back(new int(i));
  }

  // Step 2: Choose an element to erase
  auto it = myDeque.begin() + 5; // choose the 6th element to erase

  // Step 3: Erase the element
  it = myDeque.erase(it);

  // Step 4: Use assert to verify the element has been erased
  // Here we check that the size of the deque has been reduced by 1
  assert(myDeque.size() == 9);

  // Also, we can check that the 6th element is now what used to be the 7th
  assert(*myDeque[5] == 6);
  assert(**it == 6);
  }

  {
  // Test erase() with a single element
  ca::array<int, 10> deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin() + 2);
  assert((deque == ca::array<int, 10>{1, 2, 4, 5}));

  // Test erase() with a range of elements
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin() + 1, deque.begin() + 3);
  assert((deque == ca::array<int, 10>{1, 4, 5}));

  // Test erase() with a range of elements that includes the first element
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin(), deque.begin() + 2);
  assert((deque == ca::array<int, 10>{3, 4, 5}));

  // Test erase() with a range of elements that includes the last element
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin() + 3, deque.end());
  assert((deque == ca::array<int, 10>{1, 2, 3}));

  // Test erase() with a range of elements that includes the first and last elements
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin(), deque.end());
  assert(deque.empty());

  // Test erase() with a range of elements that includes the first and last elements, and the deque is empty
  deque = {};
  deque.erase(deque.begin(), deque.end());
  assert(deque.empty());
  }

  {
  // Test swap() with two deques
  ca::array<int, 10, ca::NEW> deque1 = {1, 2, 3};
  ca::array<int, 10, ca::NEW> deque2 = {4, 5, 6};
  deque1.swap(deque2);
  assert((deque1 == ca::array<int, 10>{4, 5, 6}));
  assert((deque2 == ca::array<int, 10>{1, 2, 3}));

  // Test swap() with two deques, one of which is empty
  deque1 = {1, 2, 3};
  deque2 = {};
  deque1.swap(deque2);
  assert(deque1.empty());
  assert((deque2 == ca::array<int, 10>{1, 2, 3}));

  // Test swap() with two deques, both of which are empty
  deque1 = {};
  deque2 = {};
  deque1.swap(deque2);
  assert(deque1.empty());
  assert(deque2.empty());

  // Test swap() with two deques, one of which is a subset of the other
  deque1 = {1, 2, 3};
  deque2 = {1, 2, 3, 4, 5};
  deque1.swap(deque2);
  assert((deque1 == ca::array<int, 10>{1, 2, 3, 4, 5}));
  assert((deque2 == ca::array<int, 10>{1, 2, 3}));
  }

  {
  // Test insert() with a single element
  ca::array<int, 10, ca::NEW> deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin() + 2, 6);
  assert((deque == ca::array<int, 10>{1, 2, 6, 3, 4, 5}));

  // Test insert() with a range of elements
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin() + 2, {6, 7, 8});
  assert((deque == ca::array<int, 10>{1, 2, 6, 7, 8, 3, 4, 5}));

  // Test insert() with a range of elements that includes the first element
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin(), {6, 7, 8});
  assert((deque == ca::array<int, 10>{6, 7, 8, 1, 2, 3, 4, 5}));

  // Test insert() with a range of elements that includes the last element
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.end(), {6, 7, 8});
  assert((deque == ca::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8}));

  // Test insert() with a range of elements that includes the first and last elements
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin(), {6, 7, 8});
  deque.insert(deque.end(), {9, 10, 11});
  assert((deque == ca::array<int, 10>{6, 7, 8, 1, 2, 3, 4, 5, 9, 10, 11}));

  // Test insert() with a range of elements that includes the first and last elements, and the deque is empty
  deque = {};
  deque.insert(deque.begin(), {6, 7, 8});
  deque.insert(deque.end(), {9, 10, 11});
  assert((deque == ca::array<int, 10>{6, 7, 8, 9, 10, 11}));
  }
}

int main() {
  // Run the test suite
  test1();
  test2();

  std::cout << "All tests passed!" << std::endl;

  return 0;
}
