#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    using is_transparent = void;

    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.author < rhs.author; }

    constexpr bool operator()(const Book &lhs, std::string_view rhs) const { return lhs.author < rhs; }

    constexpr bool operator()(std::string_view lhs, const Book &rhs) const { return lhs < rhs.author; }
};

struct LessByTitle {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.title < rhs.title; }
};

struct LessByYear {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.year < rhs.year; }
};

struct LessByGenre {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.genre < rhs.genre; }
};

struct LessByRating {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.rating < rhs.rating; }
};

struct LessByPopularity {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.read_count < rhs.read_count; }
};

}  // namespace bookdb::comp