#include <cassert>
#include <iostream>
#include "array.hpp" // Replace with the actual container header

// Include your testing framework of choice (e.g., Google Test or Catch2)

// Function to test various operations of your random-access container
void test1() {
  {
  // Create an instance of your container
  ca::array<int, 20> dq(ca::init_t{}, 1, 2, 3);

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
  ca::array<int, 20> dq;

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
  ca::array<int, 20> dq2 = {6, 7, 8, 9, 10};
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
  ca::array<int, 5> stack(ca::init_t{}, 1, 2, 3, 4, 5);

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
  ca::array<char, 10> palindrome(std::string_view("racecar"));

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
}

int main() {
  // Run the test suite
  test1();
  test2();

  std::cout << "All tests passed!" << std::endl;

  return 0;
}