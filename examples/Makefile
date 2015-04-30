SHELL := /bin/bash

.PHONY: all test clean

all: zc

zc: zc.cpp ../src/zstr.hpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -I../src -o $@ $^ -lz

test: zc
	diff -q zc.cpp <(./zc <zc.cpp)
	diff -q zc.cpp <(./zc -1 <zc.cpp)
	diff -q zc.cpp <(./zc -2 <zc.cpp)
	diff -q zc.cpp <(gzip <zc.cpp | ./zc)
	diff -q zc.cpp <(gzip -9 <zc.cpp | ./zc)
	diff -q <(cat zc.cpp zc.cpp) <(cat zc.cpp zc.cpp | gzip | ./zc)
	diff -q <(cat zc.cpp zc.cpp) <({ gzip <zc.cpp; gzip <zc.cpp; } | ./zc)
	@echo "all passed"
