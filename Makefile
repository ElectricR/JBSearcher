

main: main.cpp
	clang++ main.cpp `wx-config --cxxflags --libs` -std=c++20 -Wall -Wextra -Wextra-semi -O3 -o main

run: main
	./main
