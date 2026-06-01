#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

constexpr auto YearBetween(int min_year, int max_year) {
    return [min_year, max_year](const Book &book) { return book.year >= min_year && book.year <= max_year; };
}

constexpr auto RatingAbove(double min_rating) {
    return [min_rating](const Book &book) { return book.rating >= min_rating; };
}

constexpr auto GenreIs(Genre genre) {
    return [genre](const Book &book) { return book.genre == genre; };
}

template <BookIterator It, BookSentinel<It> Sent, BookPredicate Pred>
auto filterBooks(It first, Sent last, Pred pred) {
    std::vector<std::reference_wrapper<Book>> out;
    for (auto it = first; it != last; ++it) {
        if (pred(*it)) {
            out.push_back(std::ref(*it));
        }
    }
    return out;
}

template <BookPredicate... Preds>
constexpr auto all_of(Preds &&...preds) -> BookPredicate auto {
    return [&](const Book &book) { return (preds(book) && ...); };
}

template <BookPredicate... Preds>
constexpr auto any_of(Preds &&...preds) -> BookPredicate auto {
    return [&](const Book &book) { return (preds(book) && ...); };
}

}  // namespace bookdb