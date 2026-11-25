CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS = -Iinclude
.PHONY: all test clean
all: booking
booking: src/main.cpp include/booking.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp -o $@
test_booking: tests/test_booking.cpp include/booking.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_booking.cpp -o $@
test: booking test_booking
	./test_booking
	./booking < fixtures/commands.txt > actual.txt
	diff -u fixtures/expected.txt actual.txt
clean:
	rm -f booking test_booking actual.txt
