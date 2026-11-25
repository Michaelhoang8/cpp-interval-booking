#include "booking.hpp"
#include <cassert>
#include <iostream>
#include <limits>

int main() {
    BookingIndex index;
    assert(!index.reserve({"a", "room", 10, 20}));
    assert(!index.reserve({"b", "room", 20, 30}));
    assert(!index.reserve({"c", "room", 0, 10}));
    assert(index.reserve({"d", "room", 19, 21})->id == "a");
    assert(index.reserve({"e", "room", 11, 12})->id == "a");
    assert(index.reserve({"f", "room", -1, 31})->id == "c");
    assert(!index.reserve({"other", "other-room", 10, 20}));
    assert(index.list("room").size() == 3);
    assert(index.list("room")[0].id == "c");
    assert(index.cancel("a") && !index.cancel("a"));
    assert(!index.reserve({"replacement", "room", 10, 20}));
    bool rejected = false;
    try { index.reserve({"b", "other-room", 100, 200}); } catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { index.conflict("room", 1, 1); } catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    auto gaps = index.gaps("room", -5, 35);
    assert((gaps == std::vector<std::pair<std::int64_t,std::int64_t>>{{-5,0},{30,35}}));
    assert(index.gaps("room", 5, 25).empty());
    assert((index.gaps("empty", 1, 2) == std::vector<std::pair<std::int64_t,std::int64_t>>{{1,2}}));
    /* No subtraction-based comparison: extreme signed endpoints are valid. */
    assert(!index.reserve({"low", "extreme", std::numeric_limits<std::int64_t>::min(), 0}));
    assert(!index.reserve({"high", "extreme", 0, std::numeric_limits<std::int64_t>::max()}));
    assert(index.gaps("extreme", -1, 1).empty());
    std::cout << "booking tests passed\n";
}
