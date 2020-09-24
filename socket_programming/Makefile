GCC = gcc
GXX = g++

CFLAGS = -g -Wall -std=c++11

# INCLUDE LIBRARY
EXTINC = . ../../googletest/googletest/include
EXTLIB = ../../googletest/build/lib
LIBS = gtestd pthread

# UNITTEST
TESTCASES = $(shell find . -name 'test_*.cpp')
TESTOBJS = $(addsuffix .o, $(basename $(TESTCASES)))

.PHONY: all
all: main-ping server

.PHONY: test
test: test.o $(TESTOBJS) ping.o
	@echo "Building $@..."
	@$(GXX) test.o ping.o $(TESTOBJS) $(CFLAGS) \
		$(addprefix -I, $(EXTINC)) $(addprefix -L, $(EXTLIB)) \
		$(addprefix -l, $(LIBS)) -o $@

main-ping: main-ping.o ping.o
	@echo "Building $@..."
	@$(GXX) $(CFLAGS) $^ -o $@

server: server.o
	@echo "Building $@..."
	@$(GXX) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	@echo "Cleaning files..."
	@find . -name "*.[ado]" -exec rm {} \;
	@find . -name "main-ping" -exec rm {} \;
	@find . -name "server" -exec rm {} \;
	@find . -name "test" -exec rm {} \;

%.o: %.cpp
	@echo "> Compiling $^..."
	@g++ $(CFLAGS) $(addprefix -I, $(EXTINC)) -c $^ 
