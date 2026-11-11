### Final Project for CS554. 

This implementation is meant to investigate how weak the memory ordering of a concurrent queue can be while also being correct

Key details
- Fixed queue size
- One producer thread and one consumer thread
- Properties maintained
    - FIFO order
    - No reading an item before a write happens 
    - No overwriting an item before the consumer thread is done reading it

This is done with C++11 and will be teseted with the C11 memory model with CppMem
