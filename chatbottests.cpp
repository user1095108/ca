#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>

#include "array.hpp" // Replace with the actual container header

// Include your testing framework of choice (e.g., Google Test or Catch2)

// Function to test various operations of your random-access container
void test1() {
  {
  // Create an instance of your container
  dq::array<int, 20> dq(dq::multi, 1, 2, 3);

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
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

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
  dq::array<int, 10> d;

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
  dq::array<int, 10> d2;
  d2.push_back(6);
  d.swap(d2);
  assert(d.front() == 6 && d2.front() == 5);
  }

  {
  dq::array<int, 20> dq;

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

  {
  dq::array<int, 20> dq1; // default constructor
  assert(dq1.empty()); // should be empty

  dq::array<int, 20> dq2 = {1, 2, 3, 4, 5}; // initializer list constructor
  assert(!dq2.empty()); // should not be empty
  assert(dq2.size() == 5); // should contain 5 elements

  dq::array<int, 20> dq3(dq2); // copy constructor
  assert(dq3 == dq2); // should be equal to dq2

  dq::array<int, 20> dq4(std::move(dq2)); // move constructor
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
  dq::array<int, 20> dq = {1, 2, 3, 4, 5};

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
  dq::array<int, 20, dq::NEW> dq;

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
  dq::array<int, 20, dq::NEW> dq2 = {6, 7, 8, 9, 10};
  dq.swap(dq2);
  assert(dq.front() == 6);
  }

  {
  dq::array<int, 20> dq = {1, 2, 3, 4, 5};

  // Test iterators
  assert(*dq.begin() == 1);
  assert(*(dq.end() - 1) == 5);
  assert(*dq.rbegin() == 5);
  assert(*(dq.rend() - 1) == 1);

  const dq::array<int, 20> cdq = dq;
  assert(*cdq.cbegin() == 1);
  assert(*(cdq.cend() - 1) == 5);
  assert(*cdq.crbegin() == 5);
  assert(*(cdq.crend() - 1) == 1);
  }

  {
  dq::array<int, 20> dq1 = {1, 2, 3, 4, 5};
  dq::array<int, 20> dq2 = {1, 2, 3, 4, 5};
  dq::array<int, 20> dq3 = {1, 2, 3, 4, 6};

  // Test operators
  assert(dq1 == dq2);
  assert(dq1 != dq3);
  assert(dq1 < dq3);
  assert(dq1 <= dq2);
  assert(dq3 > dq1);
  assert(dq2 >= dq1);
  }

  {
  dq::array<int, 20> dq = {1, 2, 3, 4, 5};

  // Test insert
  dq.insert(dq.begin() + 2, 6);
  assert((dq == dq::array<int, 20>{1, 2, 6, 3, 4, 5}));

  // Test emplace
  dq.emplace(dq.begin() + 3, 7);
  assert((dq == dq::array<int, 20>{1, 2, 6, 7, 3, 4, 5}));

  // Test erase
  dq.erase(dq.begin() + 2, dq.begin() + 4);
  assert((dq == dq::array<int, 20>{1, 2, 3, 4, 5}));

  // Test clear
  dq.clear();
  assert(dq.empty());
  }

  {
  dq::array<int, 20> dq;

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
  dq::array<std::string, 10> dq1; // default constructor
  assert(dq1.empty()); // should be empty

  dq::array<std::string, 10> dq2 = {"one", "two", "three"}; // initializer list constructor
  assert(!dq2.empty()); // should not be empty
  assert(dq2.size() == 3); // should contain 3 elements

  dq::array<std::string, 10> dq3(dq2); // copy constructor
  assert(dq3 == dq2); // should be equal to dq2

  dq::array<std::string, 10> dq4(std::move(dq2)); // move constructor
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
  dq::array<int, 10> dq1 = {1, 2, 3};
  dq::array<int, 10> dq2 = {4, 5, 6, 7};

  // Test operator=
  dq1 = dq2;
  assert(dq1 == dq2); // dq1 should now be equal to dq2
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test operator[]
  assert(dq[0] == 1);
  assert(dq[2] == 3);
  assert(dq[4] == 5);
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test find
  auto it = std::find(dq.cbegin(), dq.cend(), 3);
  assert(it != dq.end() && *it == 3);
  it = dq::find(dq, 3);
  assert(it && *it == 3);

  it = std::find(dq.begin(), dq.end(), 6);
  assert(it == dq.end());
  assert(!dq::find(dq, 6));
  }

  {
  dq::array<int, 10> dq1 = {1, 2, 3};
  dq::array<int, 10> dq2 = {4, 5, 6, 7};

  // Test swap
  dq1.swap(dq2);
  assert((dq1 == dq::array<int, 10>{4, 5, 6, 7}));
  assert((dq2 == dq::array<int, 10>{1, 2, 3}));
  }

  {
  dq::array<int, 10> dq1 = {1, 2, 3};
  dq::array<int, 10> dq2 = {1, 2, 3};
  dq::array<int, 10> dq3 = {1, 2, 3, 4};

  // Test comparison operators
  assert(dq1 == dq2);
  assert(dq1 != dq3);
  assert(dq1 < dq3);
  assert(dq1 <= dq2);
  assert(dq3 > dq1);
  assert(dq2 >= dq1);
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test resize
  dq.resize(3);
  assert(dq.size() == 3);
  assert((dq == dq::array<int, 10>{1, 2, 3}));

  dq.resize(5);
  assert(dq.size() == 5);
  }

  {
  dq::array<int, 10> dq;

  // Test emplace_front and emplace_back
  dq.emplace_front(1);
  assert(dq.front() == 1);
  dq.emplace_back(2);
  assert(dq.back() == 2);
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test insert
  dq.insert(dq.begin() + 2, 6);
  assert((dq == dq::array<int, 10>{1, 2, 6, 3, 4, 5}));
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test emplace
  dq.emplace(dq.begin() + 2, 6);
  assert((dq == dq::array<int, 10>{1, 2, 6, 3, 4, 5}));
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

  // Test erase
  dq.erase(dq.begin() + 2, dq.begin() + 4);
  assert((dq == dq::array<int, 10>{1, 2, 5}));
  }

  {
  dq::array<int, 10> dq = {1, 2, 4, 5};
  dq::array<int, 10> to_insert = {3};

  // Test insert
  dq.insert(dq.begin() + 2, to_insert.begin(), to_insert.end());
  assert((dq == dq::array<int, 10>{1, 2, 3, 4, 5}));
  }

  {
  dq::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // Test erase
  dq.erase(dq.begin());
  assert((dq == dq::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9}));

  dq.erase(dq.begin() + 2, dq.begin() + 5);
  assert((dq == dq::array<int, 10>{1, 2, 6, 7, 8, 9}));

  //
  for (auto it = dq.begin(); it != dq.end();) {
    if (*it % 2 == 0)
        it = dq.erase(it);
    else
        ++it;
  }

  assert((dq == dq::array<int, 10>{1, 7, 9}));

  // Erase all elements
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());
  }

  {
  dq::array<int, 10> dq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // Test erase
  dq.erase(dq.begin() + 3);
  assert((dq == dq::array<int, 10>{0, 1, 2, 4, 5, 6, 7, 8, 9}));

  dq.erase(dq.end() - 1);
  assert((dq == dq::array<int, 10>{0, 1, 2, 4, 5, 6, 7, 8}));
  }

  {
  dq::array<int, 10> d1, d2;

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
  dq::array<int, 5> stack(dq::multi, 1, 2, 3, 4, 5);

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
  dq::array<int, 10> dq = {1, 2, 3, 4, 5};

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
  dq::array<char, 10> palindrome(dq::from_range, std::string_view("racecar"));

  while(palindrome.size() > 1) {
    assert(palindrome.front() == palindrome.back());
    palindrome.pop_front();
    palindrome.pop_back();
  }

  assert(1 == palindrome.size());
  }

  {
  dq::array<std::string, 52> deck;

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
  dq::array<int, 10> deque;

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
  dq::array<std::string, 10> line;

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
  dq::array<std::string, 10> magicBox;

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
  dq::array<int, 10> d;

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
  dq::array<int, 10> d = {1, 2, 3, 4, 5};
  d.erase(std::remove_if(d.begin(), d.end(), [](int n){ return n % 2 == 0; }), d.end());
  for (int n : d) assert(n % 2 != 0);
  }

  {
  dq::array<int, 10> d = {1, 2, 3, 4, 5};
  assert(2 == dq::erase_if(d, [](int n){ return n % 2 == 0; }));
  for (int n : d) assert(n % 2 != 0);
  }

  {
  dq::array<int, 20> dq{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
  dq.erase(std::unique(dq.begin(), dq.end()), dq.end());
  assert((dq == dq::array<int, 20>{1, 2, 3, 4}));
  }

  {
  dq::array<int, 10> d = {1, 2, 3, 4};
  dq::array<int, 10> l = {-1, -2, -3};
  auto pos = std::next(d.begin(), 2);
  d.insert(pos, l.begin(), l.end());
  assert((d == dq::array<int, 10>{1, 2, -1, -2, -3, 3, 4}));
  }

  {
  dq::array<int, 10> dq1{1, 2, 3, 4, 5};
  dq::array<int, 10> dq2{6, 7, 8, 9, 10};

  // Create a third deque to hold the merged result
  dq::array<int, 10> mdq(10);

  // Merge the two deques
  std::merge(dq1.begin(), dq1.end(), dq2.begin(), dq2.end(), mdq.begin());

  // Expected merged deque
  assert((mdq == dq::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

  // Reverse the merged deque
  std::reverse(mdq.begin(), mdq.end());

  // Expected reversed deque
  assert((mdq == dq::array<int, 10>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
  }

  {
  dq::array<int, 10> originalDeque{1, 2, 3, 4, 5};
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
  dq::array<int, 10> a(5, 10);

  // Check that the array has the correct size
  assert(a.size() == 5);

  // Check that all elements in the array are 10
  for (const auto& e: a) assert(e == 10);
  }

  {
  dq::array<int, 20> myDeque = {1, 2, 3, 4, 5};

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
  dq::array<int, 20> otherDeque = {8, 9, 10};
  myDeque.insert(myDeque.begin() + 2, otherDeque.begin(), otherDeque.end());
  assert(myDeque.size() == 11);
  assert(myDeque[2] == 8);
  assert(myDeque[3] == 9);
  assert(myDeque[4] == 10);
  }

  {
  dq::array<std::unique_ptr<int>, 10> myDeque;
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
  dq::array<int, 10> deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin() + 2);
  assert((deque == dq::array<int, 10>{1, 2, 4, 5}));

  // Test erase() with a range of elements
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin() + 1, deque.begin() + 3);
  assert((deque == dq::array<int, 10>{1, 4, 5}));

  // Test erase() with a range of elements that includes the first element
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin(), deque.begin() + 2);
  assert((deque == dq::array<int, 10>{3, 4, 5}));

  // Test erase() with a range of elements that includes the last element
  deque = {1, 2, 3, 4, 5};
  deque.erase(deque.begin() + 3, deque.end());
  assert((deque == dq::array<int, 10>{1, 2, 3}));

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
  dq::array<int, 10, dq::NEW> deque1 = {1, 2, 3};
  dq::array<int, 10, dq::NEW> deque2 = {4, 5, 6};
  deque1.swap(deque2);
  assert((deque1 == dq::array<int, 10>{4, 5, 6}));
  assert((deque2 == dq::array<int, 10>{1, 2, 3}));

  // Test swap() with two deques, one of which is empty
  deque1 = {1, 2, 3};
  deque2 = {};
  deque1.swap(deque2);
  assert(deque1.empty());
  assert((deque2 == dq::array<int, 10>{1, 2, 3}));

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
  assert((deque1 == dq::array<int, 10>{1, 2, 3, 4, 5}));
  assert((deque2 == dq::array<int, 10>{1, 2, 3}));
  }

  {
  // Test insert() with a single element
  dq::array<int, 10, dq::NEW> deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin() + 2, 6);
  assert((deque == dq::array<int, 10>{1, 2, 6, 3, 4, 5}));

  // Test insert() with a range of elements
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin() + 2, {6, 7, 8});
  assert((deque == dq::array<int, 10>{1, 2, 6, 7, 8, 3, 4, 5}));

  // Test insert() with a range of elements that includes the first element
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin(), {6, 7, 8});
  assert((deque == dq::array<int, 10>{6, 7, 8, 1, 2, 3, 4, 5}));

  // Test insert() with a range of elements that includes the last element
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.end(), {6, 7, 8});
  assert((deque == dq::array<int, 10>{1, 2, 3, 4, 5, 6, 7, 8}));

  // Test insert() with a range of elements that includes the first and last elements
  deque = {1, 2, 3, 4, 5};
  deque.insert(deque.begin(), {6, 7, 8});
  deque.insert(deque.end(), {9, 10, 11});
  assert((deque == dq::array<int, 10>{6, 7, 8, 1, 2, 3, 4, 5, 9, 10, 11}));

  // Test insert() with a range of elements that includes the first and last elements, and the deque is empty
  deque = {};
  deque.insert(deque.begin(), {6, 7, 8});
  deque.insert(deque.end(), {9, 10, 11});
  assert((deque == dq::array<int, 10>{6, 7, 8, 9, 10, 11}));
  }

  {
  dq::array<int, 30> arr{1, 2, 3};

  // Create a vector with elements 4, 5, 6
  std::vector<int> vec{4, 5, 6};

  // Test append_range()
  arr.append_range(vec);
  assert((arr == dq::array<int, 30>{1, 2, 3, 4, 5, 6}));

  // Test prepend_range()
  arr.prepend_range(vec);
  assert((arr == dq::array<int, 30>{4, 5, 6, 1, 2, 3, 4, 5, 6}));

  // Test insert_range()
  auto it = arr.begin();
  std::advance(it, 3); // Move iterator to the 4th element
  arr.insert_range(it, vec);
  assert((arr == dq::array<int, 30>{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));
  }

  {
  dq::array<int, 20> dq = {1, 2, 3, 4, 5};

  // Resize the deque to 3 elements
  dq.resize(3);

  // Check the size of the deque
  assert(dq.size() == 3);

  // Check the elements of the deque
  assert(dq[0] == 1);
  assert(dq[1] == 2);
  assert(dq[2] == 3);

  // Resize the deque to 5 elements
  dq.resize(5, 100);

  // Check the size of the deque
  assert(dq.size() == 5);

  // Check the elements of the deque
  assert(dq[0] == 1);
  assert(dq[1] == 2);
  assert(dq[2] == 3);
  assert(dq[3] == 100);
  assert(dq[4] == 100);
  }

  {
  dq::array<int, 20> myDeque;

  // Adding elements to the deque
  myDeque.push_back(10);
  myDeque.push_front(5);
  myDeque.push_back(20);

  // Checking the size of the deque
  assert(myDeque.size() == 3);

  // Accessing elements using iterators
  auto it = myDeque.begin();
  assert(*it == 5);

  // Modifying elements
  *it = 15;
  assert(myDeque.front() == 15);

  // Removing elements
  myDeque.pop_front();
  assert(myDeque.front() == 10);

  // Clearing the deque
  myDeque.clear();
  assert(myDeque.empty());
  }

  {
  // Test Case 1: Basic functionality
  dq::array<int, 20> deque1 = {1, 2, 3};
  assert(deque1.size() == 3);
  assert(deque1[0] == 1);
  assert(deque1[1] == 2);
  assert(deque1[2] == 3);

  // Test Case 2: Adding elements to the front and back
  deque1.push_front(0);
  deque1.push_back(4);
  assert(deque1.size() == 5);
  assert(deque1[0] == 0);
  assert(deque1[4] == 4);

  // Test Case 3: Iterating through the deque
  int sum = 0;
  for (const auto& elem : deque1) {
      sum += elem;
  }
  assert(sum == 10);

  // Test Case 4: Erasing elements
  deque1.erase(deque1.begin() + 2); // Erase element at index 2
  assert(deque1.size() == 4);
  assert(deque1[2] == 3);

  // Test Case 5: Clearing the deque
  deque1.clear();
  assert(deque1.empty());

  // Test Case 6: Emplace elements
  deque1.emplace_back(10);
  deque1.emplace_front(5);
  assert(deque1.size() == 2);
  assert(deque1[0] == 5);
  assert(deque1[1] == 10);
  assert(dq::find(deque1, 10));
  assert(dq::erase(deque1, 5, 10));
  assert(deque1.empty());
  assert(!dq::find(deque1, 10));
  }

  {
  dq::array<int, 10> d{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  
  // Use std::erase to remove all elements equal to 5
  dq::erase(d, 5);
  assert(std::find(d.begin(), d.end(), 5) == d.end());
  assert(!dq::find(d, 5));

  // Use std::erase_if to remove all even numbers
  dq::erase_if(d, [](int n){ return n % 2 == 0; });
  assert(std::none_of(d.begin(), d.end(), [](int n){ return n % 2 == 0; }));
  assert(!dq::find_if(d, [](int n){ return n % 2 == 0; }));
  }

  {
  dq::array<int, 10> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  dq::erase_if(d, [](int i){ return i % 2 == 0; });
  assert(d.size() == 5);
  assert((d == dq::array<int, 10>{1, 3, 5, 7, 9}));

  d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  dq::erase(d, 5);
  assert(d.size() == 9);
  assert((d == dq::array<int, 10>{1, 2, 3, 4, 6, 7, 8, 9, 10}));
  }

  {
  dq::array<int, 10> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  // Test 1: Erase from beginning to middle
  dq.erase(dq.begin(), dq.begin() + 5);
  assert(dq.size() == 5);
  assert((dq == decltype(dq){6, 7, 8, 9, 10}));

  // Test 2: Erase from middle to end
  dq.erase(dq.begin() + 1, dq.end());
  assert(dq.size() == 1);
  assert(dq.front() == 6);

  // Test 3: Erase entire deque
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  // Test 4: Erase from empty deque
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  // Test 5: Erase from one element
  dq.push_back(1);
  dq.erase(dq.begin(), dq.begin() + 1);
  assert(dq.empty());

  // Test 6: Erase from one element to end
  dq.push_back(1, 2);
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  // Test 7: Erase from begin to end of one element
  dq.push_back(1);
  dq.erase(dq.begin());
  assert(dq.empty());

  // Test 9: Erase from begin to end of three elements
  dq.push_back(1, 2, 3);
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  // Test 10: Erase from begin to end of four elements
  dq.push_back(1, 2, 3, 4);
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  dq.erase(dq.begin() + 2, dq.begin() + 5);
  assert(dq.size() == 7);  // The size should now be 7.
  }

  {
  dq::array<int, 10> dq{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  // Test 1: Erase a range of elements from the deque
  dq.erase(dq.begin() + 2, dq.begin() + 5);
  assert((dq == decltype(dq){1, 2, 6, 7, 8, 9, 10}));

  // Test 2: Erase the entire deque
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  // Test 3: Erase a single element from the deque
  dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  dq.erase(dq.begin() + 4); // Erase '5'
  assert((dq == decltype(dq){1, 2, 3, 4, 6, 7, 8, 9, 10}));

  // Test 4: Erase a range with the same start and end iterator
  auto it = dq.begin() + 3;
  dq.erase(it, it);
  assert((dq == decltype(dq){1, 2, 3, 4, 6, 7, 8, 9, 10}));

  // Test 5: Erase a range that encompasses the entire deque
  dq.erase(dq.begin(), dq.end());
  assert(dq.empty());

  // Test 6: Erase an empty range from the deque
  dq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  dq.erase(dq.end(), dq.end());
  assert((dq == decltype(dq){1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
  }

  {
    int const a[10]{};
    assert(std::ranges::equal(a, dq::array<int, 10>(a)));
  }

  { // test_ranges
    dq::array<int, 20> dq = {3, 1, 4, 1, 5};

    // ranges::sort works with our iterator/sentinel pair
    std::ranges::sort(dq);
    assert(std::ranges::is_sorted(dq));
    assert(std::ranges::equal(dq, std::array{1, 1, 3, 4, 5}));

    // ranges::contains
    assert(std::ranges::find(dq, 3) != dq.end());
    assert(std::ranges::count(dq, 1) == 2);
  }

  { // test_move_only
    dq::array<std::unique_ptr<int>, 10> dq;
    dq.emplace_back(std::make_unique<int>(7));
    dq.emplace_front(std::make_unique<int>(9));

    assert(*dq.front() == 9);
    assert(*dq.back()  == 7);

    // erase in the middle
    dq.erase(dq.begin()); // destroy the 9
    assert(dq.size() == 1);
    assert(*dq.front() == 7);
  }

  { // test_capacity_edge
    dq::array<int, 4> dq{0, 1, 2, 3};

    assert(dq.full());
    assert(dq.size() == 4);

    // push_back when full -> oldest element (0) must be overwritten
    dq.push_back(42);
    assert(dq.full());
    assert(dq.size() == 4);
    assert(dq.back()  == 42);
    assert(dq.front() == 1);        // 0 has been popped

    // push_front when full
    dq.push_front(99);
    assert(dq.full());
    assert(dq.front() == 99);
    assert(dq.back()  == 42);
  }

  { // test_heterogeneous_lookup
    dq::array<std::string, 10> dq = {"apple", "banana", "pear"};
    // transparent operator== via std::string_view
    assert(std::ranges::find(dq, std::string_view("banana")) != dq.end());
    assert(std::ranges::find(dq, std::string_view("grape")) == dq.end());
  }

  { // test_iterator_stability
    dq::array<int, 20> dq = {0,1,2,3,4,5};
    auto it = dq.begin() + 3; // points at 3
    auto dist = std::distance(dq.begin(), it);

    dq.insert(it, 42);        // invalidate it
    it = dq.begin() + dist;   // rebuild
    assert(*it == 42);

    it = dq.erase(it);        // erase returns iterator *after* the removed one
    assert(*it == 3);
  }

  { // test_stress
    constexpr int N = 1'000;
    dq::array<int, N> dq;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> op(0, 3); // 0-push_back, 1-push_front, 2-pop_back, 3-pop_front

    for (int i = 0; i < 10 * N; ++i)
    {
      switch (op(rng))
      {
        case 0: dq.push_back(i); break;
        case 1: dq.push_front(i); break;
        case 2: if (!dq.empty()) dq.pop_back(); break;
        case 3: if (!dq.empty()) dq.pop_front(); break;
      }
      assert(dq.size() <= N);
    }
  }

  { // test_ranges_projection
    struct Point { int x, y; };
    dq::array<Point, 10> dq = {{5,6}, {1,2}, {3,4}};

    // sort by y-coordinate via projection
    std::ranges::sort(dq, std::less<>{}, &Point::y);
    assert(dq[0].y == 2 && dq[2].y == 6);
  }

  { // test_const_iterators
    const dq::array<int, 5> dq = {10,20,30};
    static_assert(std::same_as<decltype(dq.begin()), dq::array<int,5>::const_iterator>);
    assert(std::accumulate(dq.begin(), dq.end(), 0) == 60);
  }

  { // test_resize_value_init
    dq::array<int, 10> dq = {1,2,3};
    dq.resize(6, 42);        // expand with value
    assert(dq.size() == 6);
    assert(dq[3] == 42 && dq[5] == 42);

    dq.resize(2);            // shrink
    assert(dq.size() == 2);
  }

  { // test_self_swap
    dq::array<int, 5> dq = {1,2,3};
    dq.swap(dq);             // must be no-op
    assert(dq.size() == 3 && dq[0] == 1);
  }

  { // test_reverse_ranges
    dq::array<int, 10> dq = {1,2,3,2,1};
    auto rfirst = dq.rbegin(), rlast = dq.rend();
    assert(std::ranges::count(rfirst, rlast, 2) == 2);
  }

  { // test_erase_if_capture
    dq::array<int, 10> dq = {1,2,3,4,5,6};
    int threshold = 4;
    dq::erase_if(dq, [threshold](int x){ return x > threshold; });
    assert((dq == dq::array<int,10>{1,2,3,4}));
  }

  { // test_spsc
    dq::array<int, 10> buffer;
    std::atomic<bool> done{};
    std::mutex m;
    int consumed{};

    std::thread producer([&]{
      for (int i = 0; i < 1000;)
      {
        if (std::lock_guard l(m); !buffer.full())
          buffer.push_back(i++);
      }
      done = true;
    });

    std::thread consumer([&]{
      for (;;)
      {
        if (std::lock_guard l(m); !buffer.empty())
          consumed += buffer.front(), buffer.pop_front();
        else if (done) break;
      }
    });

    producer.join();
    consumer.join();

    assert(consumed == 999*1000/2); // 0+1+...+999
  }

  { // test_insert_raw_array
    int raw[] = {9,8,7};
    dq::array<int, 10> dq = {1,2,3};
    dq.insert(dq.begin()+1, std::begin(raw), std::end(raw));
    assert((dq == dq::array<int,10>{1,9,8,7,2,3}));
  }

  { // test_assign_init_list
    dq::array<int, 10> dq;
    dq.assign({10,20,30});
    assert((dq == dq::array<int,10>{10,20,30}));
    assert(dq.size() == 3);
  }

  { // test_std_swap
    dq::array<std::string, 5> a = {"a","b"};
    dq::array<std::string, 5> b = {"x","y","z"};
    std::swap(a,b);
    assert((a == dq::array<std::string,5>{"x","y","z"}));
    assert((b == dq::array<std::string,5>{"a","b"}));
  }

  { // test_inner_product
    dq::array<int, 5> a = {1,2,3};
    dq::array<int, 5> b = {4,5,6};
    int dot = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    assert(dot == 32); // 1*4 + 2*5 + 3*6
  }

  {
    // test_reverse_exact
    dq::array<int, 6> dq = {0,1,2,3,4,5};
    std::vector<int> expected = {5,4,3,2,1,0};
    std::vector<int> actual;
    std::copy(dq.rbegin(), dq.rend(), std::back_inserter(actual));
    assert(actual == expected);
  }

  { // test_lex_compare
    dq::array<int, 20> dq = {1,2,3};
    std::vector<int> v = {1,2,3,4};

    assert(std::ranges::lexicographical_compare(dq, v)); // < 0
    assert(!std::ranges::lexicographical_compare(v, dq)); // > 0
    assert(!std::ranges::lexicographical_compare(dq, dq)); // == 0
  }

  { // test_fill_then_modify
    dq::array<int, 10> dq(7, 42);
    assert(dq.size() == 7);
    assert(std::all_of(dq.begin(), dq.end(), [](int x){ return x==42; }));

    dq[3] = 99;
    assert(dq[3] == 99);
    assert(std::count(dq.begin(), dq.end(), 42) == 6);
  }

  { // test_front_back_mirror
    dq::array<char, 4> dq;
    for (char c = 'a'; c <= 'z'; ++c)
    {
      dq.push_back(c);
      if (dq.size() > 1)
        assert(dq.front() == char('a' + (c - 'a') - (dq.size() - 1)));
    }
  }

  { // test_shrink
    dq::array<int, 100> dq(std::ranges::iota_view{0, 50});
    dq.resize(10); // downsize
    assert(dq.size() == 10);
    assert(dq.front() == 0 && dq.back() == 9);
  }

  { // test_ranges_accumulate
    dq::array<int, 10> dq = {1,2,3,4,5};
    int sum = std::reduce(std::execution::unseq, dq.begin(), dq.end(), 0);
    assert(sum == 15);
  }

  { // test_back_forth_equivalence
    dq::array<int, 8> a = {1,2,3,4,5,6};
    auto b = a;

    // push_front / pop_back 6 times must restore original order
    for (unsigned i = 0; i < 6; ++i)
    {
      auto const t(b.back());
      b.pop_back();
      b.push_front(t);
    }
    assert(a == b);
  }

  { // test_rotate
    dq::array<int, 10> dq = {1,2,3,4,5};
    std::rotate(dq.begin(), dq.begin()+2, dq.end()); // 3,4,5,1,2
    assert((dq == dq::array<int,10>{3,4,5,1,2}));

    std::rotate(dq.rbegin(), dq.rbegin()+2, dq.rend()); // reverse rotate
    assert((dq == dq::array<int,10>{1,2,3,4,5}));
  }

  {
    // Test with std::string (non-trivial type) and full capacity overwrite
    dq::array<std::string, 5> dq;
    dq.push_back("hello");
    dq.push_back("world");
    dq.push_front("start");
    assert(dq.size() == 3);
    assert(dq[0] == "start");
    assert(dq[1] == "hello");
    assert(dq[2] == "world");

    dq.pop_front();
    assert(dq.front() == "hello");

    // Test full capacity with push_back
    dq = {"a", "b", "c", "d", "e"};
    assert(dq.full());
    dq.push_back("f"); // should remove "a"
    assert(dq.size() == 5);
    assert(dq[0] == "b");
    assert(dq[4] == "f");

    // Test full capacity with push_front
    dq = {"a", "b", "c", "d", "e"};
    dq.push_front("f"); // should remove "e"
    assert(dq.size() == 5);
    assert(dq[0] == "f");
    assert(dq[4] == "e");
  }

  {
    // Test with std::unique_ptr (move-only type)
    dq::array<std::unique_ptr<int>, 3> dq;
    dq.push_back(std::make_unique<int>(1));
    dq.push_back(std::make_unique<int>(2));
    dq.push_front(std::make_unique<int>(3));

    assert(*dq[0] == 3);
    assert(*dq[1] == 1);
    assert(*dq[2] == 2);

    // Move out an element and test container behavior
    auto p = std::move(dq[1]);
    assert(*p == 1);
    assert(!dq[1]);
    dq.pop_back();
    assert(dq.size() == 2);
    dq.pop_back();
    assert(dq.size() == 1);
    assert(*dq[0] == 3);
  }

  {
    // Test self-assignment
    dq::array<int, 5> dq = {1, 2, 3};
    auto& dq_ref = dq;
    dq = dq_ref;
    assert(dq.size() == 3);
    assert(dq[0] == 1);
  }

  {
    // Test self-swap
    dq::array<int, 5, dq::NEW> dq = {1, 2, 3};
    dq.swap(dq);
    assert(dq.size() == 3);
    assert(dq[0] == 1);
  }

  {
    // Test range assignment
    std::vector<int> v = {10, 20, 30, 40};
    dq::array<int, 10> dq;
    dq.assign(v.begin(), v.end());
    assert(dq.size() == 4);
    assert(dq[0] == 10);
    assert(dq[3] == 40);
  }

  {
    // Test for memory leaks using a counter type
    static int count;
    struct Counter {
      int x;
      Counter() { ++count; }
      Counter(int x_) : x(x_) { ++count; }
      ~Counter() { --count; }
      Counter(const Counter& other) : x(other.x) { ++count; }
      Counter(Counter&& other) : x(other.x) { ++count; }
      Counter& operator=(const Counter&) = default;
      Counter& operator=(Counter&&) = default;
    };

    {
      dq::array<Counter, 10> dq;
      dq.push_back(1);
      dq.push_back(2);
      assert(count == 11);
      dq.pop_back();
      assert(count == 11);
      dq.clear();
      assert(count == 11);
    }
    assert(count == 0);
  }

  {
    // Stress test with mixed operations
    dq::array<int, 100> dq;
    for (int i = 0; i < 10000; ++i) {
      switch (i % 5) {
        case 0: dq.push_back(i); break;
        case 1: dq.push_front(i); break;
        case 2: if (!dq.empty()) dq.pop_back(); break;
        case 3: if (!dq.empty()) dq.pop_front(); break;
        case 4: if (!dq.empty()) dq.erase(dq.begin() + (i % dq.size())); break;
      }
    }
    assert(dq.size() <= 100);
  }

  {
    // Test heterogeneous lookup with std::string_view
    dq::array<std::string, 5> dq = {"apple", "banana", "cherry"};
    auto it = dq::find(dq, "banana");
    assert(it && *it == "banana");
    it = dq::find(dq, std::string_view("cherry"));
    assert(it && *it == "cherry");
    dq::erase(dq, "apple", "banana", "cherry");
    assert(dq.empty());
  }
}

int main() {
  // Run the test suite
  test1();

  std::cout << "All tests passed!" << std::endl;

  return 0;
}
