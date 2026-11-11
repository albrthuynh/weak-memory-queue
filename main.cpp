#include <cassert>
#include <iostream>
#include <vector>

// Circular Queue
class Queue {
public:
  Queue(std::size_t size) : values(size), front(0), back(0), curr_size(0) {
    // elements with 0 capacity cannot accept anything
    if (size == 0) {
      std::cout << "Size must be positive" << std::endl;
      return;
    }
  }

  // adds to the back of the queue, returns true if successful and false otherwise
  bool add(int new_val) {
    if (curr_size == values.size()) {
      std::cout << "Queue is full, cannot add more items" << std::endl;
      return false;
    }

    values[back] = new_val;
    back = (back + 1) % values.size();
    curr_size++;

    return true;
  }

  // removes from the front of the queue, returns true if successful and false otherwise
  bool remove(int& removed_val) {
    if (curr_size <= 0) {
      std::cout << "The queue has no elements to remove" << std::endl;
      return false;
    }

    removed_val = values[front];
    front = (front + 1) % values.size();
    curr_size--;

    return true;
  }

private:
  std::vector<int> values;
  std::size_t front;
  std::size_t back;
  std::size_t curr_size;
};

int main() {
  Queue queue(3);
  int value = -1;

  // Empty queue
  assert(!queue.remove(value));

  // Fill the queue
  assert(queue.add(10));
  assert(queue.add(20));
  assert(queue.add(30));

  // Full queue: adding must fail without changing its contents
  assert(!queue.add(40));

  // Remove the oldest item
  assert(queue.remove(value));
  assert(value == 10);

  // Reuse the freed slot: insertion wraps around
  assert(queue.add(40));

  // Check FIFO order, including removal wrapping around
  assert(queue.remove(value));
  assert(value == 20);

  assert(queue.remove(value));
  assert(value == 30);

  assert(queue.remove(value));
  assert(value == 40);

  // Empty again
  assert(!queue.remove(value));

  // Reuse after draining
  assert(queue.add(50));
  assert(queue.remove(value));
  assert(value == 50);

  // Capacity of one
  Queue single(1);
  assert(single.add(7));
  assert(!single.add(8));
  assert(single.remove(value));
  assert(value == 7);
  assert(!single.remove(value));

  // Zero capacity
  Queue zero(0);
  assert(!zero.add(1));
  assert(!zero.remove(value));

  std::cout << "All queue tests passed!\n";
  return 0;
}
