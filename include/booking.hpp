#pragma once
#include <cstdint>
#include <iterator>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Booking {
    std::string id, resource;
    std::int64_t start, end;
};

class BookingIndex {
    using Schedule = std::map<std::int64_t, Booking>;
    std::map<std::string, Schedule> resources_;
    std::map<std::string, std::pair<std::string, std::int64_t>> identifiers_;

    static void validate(std::int64_t start, std::int64_t end) {
        if (start >= end) throw std::invalid_argument("start must be less than end");
    }

public:
    // Half-open intervals [start,end): an ending time may equal the next start.
    std::optional<Booking> conflict(const std::string& resource,
                                    std::int64_t start, std::int64_t end) const {
        validate(start, end);
        auto found = resources_.find(resource);
        if (found == resources_.end()) return std::nullopt;
        const auto& schedule = found->second;
        auto next = schedule.lower_bound(start);
        if (next != schedule.begin()) {
            const auto& previous = std::prev(next)->second;
            if (previous.end > start) return previous;
        }
        if (next != schedule.end() && next->second.start < end) return next->second;
        return std::nullopt;
    }

    // Returns one conflicting booking without mutating either index.
    std::optional<Booking> reserve(Booking booking) {
        validate(booking.start, booking.end);
        if (booking.id.empty() || booking.resource.empty()) throw std::invalid_argument("id/resource cannot be empty");
        if (identifiers_.count(booking.id)) throw std::invalid_argument("duplicate booking id");
        if (auto existing = conflict(booking.resource, booking.start, booking.end)) return existing;
        auto& schedule = resources_[booking.resource];
        auto [position, inserted] = schedule.emplace(booking.start, booking);
        if (!inserted) throw std::logic_error("inconsistent schedule");
        try {
            identifiers_.emplace(booking.id, std::make_pair(booking.resource, booking.start));
        } catch (...) {
            schedule.erase(position);
            throw;
        }
        return std::nullopt;
    }

    bool cancel(const std::string& id) {
        auto found = identifiers_.find(id);
        if (found == identifiers_.end()) return false;
        auto resource = resources_.find(found->second.first);
        resource->second.erase(found->second.second);
        if (resource->second.empty()) resources_.erase(resource);
        identifiers_.erase(found);
        return true;
    }

    std::vector<Booking> list(const std::string& resource) const {
        std::vector<Booking> result;
        auto found = resources_.find(resource);
        if (found != resources_.end()) {
            for (const auto& entry : found->second) result.push_back(entry.second);
        }
        return result;
    }

    std::vector<std::pair<std::int64_t, std::int64_t>> gaps(
        const std::string& resource, std::int64_t start, std::int64_t end) const {
        validate(start, end);
        std::vector<std::pair<std::int64_t, std::int64_t>> result;
        auto found = resources_.find(resource);
        if (found == resources_.end()) return {{start, end}};
        const auto& schedule = found->second;
        auto at = schedule.lower_bound(start);
        if (at != schedule.begin() && std::prev(at)->second.end > start) --at;
        auto cursor = start;
        for (; at != schedule.end() && at->second.start < end; ++at) {
            const auto& booking = at->second;
            if (cursor < booking.start) result.emplace_back(cursor, booking.start);
            if (booking.end > cursor) cursor = booking.end;
            if (cursor >= end) break;
        }
        if (cursor < end) result.emplace_back(cursor, end);
        return result;
    }
};
