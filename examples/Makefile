SHELL := /bin/bash

.PHONY: all test clean

all: zpipe ztxtpipe

%: %.cpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -I../src -o $@ $^ -lz

test: zpipe ztxtpipe
	diff -q ztxtpipe.cpp <(./ztxtpipe <ztxtpipe.cpp)
	diff -q ztxtpipe.cpp <(gzip <ztxtpipe.cpp | ./ztxtpipe)
	diff -q ztxtpipe.cpp <(gzip -9 <ztxtpipe.cpp | ./ztxtpipe)
	diff -q <(cat ztxtpipe.cpp ztxtpipe.cpp) <(cat ztxtpipe.cpp ztxtpipe.cpp | gzip | ./ztxtpipe)
	diff -q <(cat ztxtpipe.cpp ztxtpipe.cpp) <({ gzip <ztxtpipe.cpp; gzip <ztxtpipe.cpp; } | ./ztxtpipe)
	diff -q zpipe.cpp <(./zpipe <zpipe.cpp)
	diff -q zpipe.cpp <(gzip <zpipe.cpp | ./zpipe)
	diff -q zpipe.cpp <(gzip -9 <zpipe.cpp | ./zpipe)
	diff -q <(cat zpipe.cpp zpipe.cpp) <(cat zpipe.cpp zpipe.cpp | gzip | ./zpipe)
	diff -q <(cat zpipe.cpp zpipe.cpp) <({ gzip <zpipe.cpp; gzip <zpipe.cpp; } | ./zpipe)
	diff -q <(<zpipe.cpp | gzip) <(<zpipe.cpp | gzip | gzip | ./zpipe)
	@echo "all passed"

clean:
	rm -rf zpipe ztxtpipe
