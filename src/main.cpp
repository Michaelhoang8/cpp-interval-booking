#include "booking.hpp"
#include <iostream>
#include <sstream>

int main() {
    BookingIndex index;
    std::string line;
    std::size_t number = 0;
    while (std::getline(std::cin, line)) {
        ++number;
        if (line.empty() || line[0] == '#') continue;
        try {
            std::istringstream input(line);
            std::string command, id, resource, extra;
            std::int64_t start = 0, end = 0;
            input >> command;
            if (command == "book") {
                if (!(input >> id >> resource >> start >> end) || input >> extra) throw std::invalid_argument("book ID RESOURCE START END");
                auto conflict = index.reserve({id, resource, start, end});
                std::cout << (conflict ? "conflict " + conflict->id : "booked " + id) << '\n';
            } else if (command == "cancel") {
                if (!(input >> id) || input >> extra) throw std::invalid_argument("cancel ID");
                std::cout << (index.cancel(id) ? "cancelled " : "missing ") << id << '\n';
            } else if (command == "gaps") {
                if (!(input >> resource >> start >> end) || input >> extra) throw std::invalid_argument("gaps RESOURCE START END");
                for (auto [from, to] : index.gaps(resource, start, end)) std::cout << "gap " << from << ' ' << to << '\n';
            } else if (command == "list") {
                if (!(input >> resource) || input >> extra) throw std::invalid_argument("list RESOURCE");
                for (const auto& booking : index.list(resource)) std::cout << booking.id << ' ' << booking.start << ' ' << booking.end << '\n';
            } else throw std::invalid_argument("unknown command");
        } catch (const std::exception& error) {
            std::cerr << "line " << number << ": " << error.what() << '\n';
            return 2;
        }
    }
    return std::cin.bad() ? 1 : 0;
}
