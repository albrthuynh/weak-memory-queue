#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

// Circular Queue
class Queue {
public:
  Queue(std::size_t size) : values(size + 1), front(0), back(0) {
    // elements with 0 capacity cannot accept anything
    if (size == 0) {
      // std::cout << "Size must be positive" << std::endl;
      return;
    }
  }

  // adds to the back of the queue, returns true if successful and false otherwise
  bool add(int new_val) {
    std::size_t curr_back = back.load(std::memory_order_seq_cst);
    std::size_t next_back = (curr_back + 1) % values.size();
    std::size_t curr_front = front.load(std::memory_order_seq_cst);

    // we know if it's a full queue if the next_back index is equal to the curr_front
    if (next_back == curr_front) {
      // std::cout << "The queue is full" << std::endl;
      return false;
    }

    values[curr_back] = new_val;
    back.store(next_back, std::memory_order_seq_cst);

    return true;
  }

  // removes from the front of the queue, returns true if successful and false otherwise
  bool remove(int& removed_val) {
    std::size_t curr_front = front.load(std::memory_order_seq_cst);
    std::size_t curr_back = back.load(std::memory_order_seq_cst);

    if (curr_front == curr_back) {
      // std::cout << "The queue has no elements to remove" << std::endl;
      return false;
    }

    removed_val = values[curr_front];
    std::size_t next_front = (curr_front + 1) % values.size();
    front.store(next_front, std::memory_order_seq_cst);

    return true;
  }

private:
  std::vector<int> values;
  std::atomic<std::size_t> front;
  std::atomic<std::size_t> back;
};

void test_producer_consumer() {
  Queue queue(3);
  const int num_items = 10000;

  std::thread producer([&queue]() {
    for (int item = 0; item < num_items; item++) {
      // Retry the same item until insertion succeeds.
      while (!queue.add(item)) {
        std::this_thread::yield(); // tells the scheduler to let the other thread run
      }
    }
  });

  std::thread consumer([&queue]() {
    for (int expected = 0; expected < num_items; expected++) {
      int value = -1;

      // Wait until an item is available.
      // Value gets updated here because it is pass by reference
      while (!queue.remove(value)) {
        std::this_thread::yield();
      }

      // Assert condition
      assert(value == expected);
    }
  });

  producer.join();
  consumer.join();

  // Both threads finished; all items should have been consumed.
  int value = -1;
  assert(!queue.remove(value));

  std::cout << "Producer/consumer test passed!\n";
}

int main() {
  Queue queue(3);
  int value = -1;

  test_producer_consumer();
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
