#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T container, const Book &book, Book &&rvalBook) {
    requires std::same_as<typename T::value_type, Book>;
    { container.begin() } -> std::input_iterator;
    { container.end() } -> std::sentinel_for<decltype(container.begin())>;
    { container.rbegin() } -> std::input_iterator;
    { container.rend() } -> std::sentinel_for<decltype(container.rbegin())>;
    { container.size() } -> std::convertible_to<std::size_t>;
    { container.empty() } -> std::convertible_to<bool>;
    requires std::ranges::contiguous_range<T>;

    { container.clear() } noexcept;

    container.push_back(book);
    container.push_back(std::move(rvalBook));

    container.emplace_back(book);
    container.emplace_back(std::move(rvalBook));
};

template <typename T>
concept BookIterator = std::input_iterator<T> && std::same_as<std::iter_value_t<T>, Book>;

template <typename Sent, typename It>
concept BookSentinel = BookIterator<It> && std::sentinel_for<Sent, It>;

template <typename P>
concept BookPredicate = std::predicate<P, const Book &>;

template <typename C>
concept BookComparator = std::strict_weak_order<C, const Book &, const Book &>;

}  // namespace bookdb