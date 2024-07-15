all:test main

main:main.cpp
	g++ -o $@ $^ -g -lpthread

test:test.cpp
	g++ -o $@ $^ -g -std=c++11 -lpthread

.PHONY::clean
clean:
	rm -f test
	rm -f main
	rm -rf logfile