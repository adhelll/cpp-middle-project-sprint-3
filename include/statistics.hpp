#pragma once

#include <algorithm>
#include <cstddef>
#include <flat_map>
#include <iterator>
#include <numeric>
#include <random>
#include <string_view>

#include "book.hpp"
#include "book_database.hpp"

#include <vector>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string_view, size_t, Comparator> hist(comp);

    const auto &books = cont.GetBooks();

    for (const auto &book : books) {
        auto [it, _] = hist.try_emplace(book.author, 0);
        it->second++;
    }

    return hist;
}

template <BookIterator It, BookSentinel<It> Sent>
auto calculateGenreRatings(It first, Sent last) {
    struct RatingCount {
        double sum = 0.0;
        size_t count = 0;
    };

    std::flat_map<Genre, RatingCount> stats;

    for (auto it = first; it != last; ++it) {
        const auto &book = *it;
        auto [mapIt, _] = stats.try_emplace(book.genre);
        mapIt->second.sum += book.rating;
        mapIt->second.count++;
    }

    std::flat_map<Genre, double> ratingSums;
    for (const auto &[genre, stat] : stats) {
        ratingSums.emplace_hint(ratingSums.end(), genre, stat.sum / stat.count);
    }

    return ratingSums;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &books) {
    if (books.empty()) {
        return {};
    }

    const auto sumRating = std::transform_reduce(books.begin(), books.end(), 0.0, std::plus<double>(),
                                                 [](const auto &book) { return book.rating; });

    return sumRating / static_cast<double>(books.size());
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &cont, size_t num) {
    static std::mt19937 gen(std::random_device{}());
    std::vector<std::reference_wrapper<const Book>> out;
    out.reserve(num);

    std::sample(cont.begin(), cont.end(), std::back_inserter(out), num, gen);
    return out;
}

template <BookContainerLike T, typename Comparator>
auto getTopNBy(BookDatabase<T> &cont, size_t n, Comparator comp) {
    std::vector<std::reference_wrapper<const Book>> out(cont.begin(), cont.end());

    n = std::min(n, out.size());

    std::partial_sort(out.begin(), out.begin() + n, out.end(),
                      [&comp](const auto &a, const auto &b) { return !comp(a.get(), b.get()); });

    out.erase(out.begin() + n, out.end());
    return out;
}

}  // namespace bookdb
