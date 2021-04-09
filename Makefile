main: src/main.cpp
	clang++ src/main.cpp `wx-config --cxxflags --libs` -std=c++20 -Wall -Wextra -Wextra-semi -Wl,-rpath="./lib" -O3 -o main

test: tests/test_algo.cpp
	clang++ tests/test_algo.cpp -std=c++20 -lpthread -lgtest -lgtest_main -Wall -Wextra -Wextra-semi -O3 -o test_algo

run: main
	./main
