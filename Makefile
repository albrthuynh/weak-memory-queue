.PHONY: run clean

queue_test: main.cpp
	g++ -std=c++11 -Wall -Wextra -pthread main.cpp -o queue_test

run: queue_test
	./queue_test

clean:
	rm -f queue_test
