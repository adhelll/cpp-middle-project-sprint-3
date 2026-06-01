#pragma once

#include <functional>
#include <initializer_list>
#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    using value_type = Book;
    using reference = Book &;
    using const_reference = const Book &;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;
    using reverse_iterator = typename BookContainer::reverse_iterator;
    using const_reverse_iterator = typename BookContainer::const_reverse_iterator;
    using size_type = typename BookContainer::size_type;

    BookDatabase() = default;
    BookDatabase(std::initializer_list<Book> init) : books_(init) {
        for (auto &book : books_) {
            resolveAuthor(book);
        }
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    void PushBack(const Book &book) {
        books_.push_back(book);
        resolveAuthor(books_.back());
    }

    void PushBack(Book &&book) {
        books_.push_back(std::move(book));
        resolveAuthor(books_.back());
    }

    template <typename... Args>
        requires std::constructible_from<Book, Args...>
    void EmplaceBack(Args &&...args) {
        books_.emplace_back(std::forward<Args>(args)...);
        resolveAuthor(books_.back());
    }

    std::span<const Book> GetBooks() const { return books_; }
    std::span<Book> GetBooks() { return books_; }
    const AuthorContainer &GetAuthors() { return authors_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

    size_type size() const noexcept { return books_.size(); }
    bool empty() const noexcept { return size() == 0; }

    iterator begin() { return books_.begin(); }
    const_iterator begin() const { return books_.begin(); }
    iterator end() { return books_.end(); }
    const_iterator end() const { return books_.end(); }

    reverse_iterator rbegin() { return books_.rbegin(); }
    const_reverse_iterator rbegin() const { return books_.rbegin(); }
    reverse_iterator rend() { return books_.rend(); }
    const_reverse_iterator rend() const { return books_.rend(); }

private:
    void resolveAuthor(Book &book) {
        auto [it, inserted] = authors_.emplace(std::string(book.author));

        book.author = std::string_view(*it);
    }

    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {

template <typename BookContainer>
struct formatter<bookdb::BookDatabase<BookContainer>> {

    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<BookContainer> &db, FormatContext &fc) const {
        auto out = fc.out();
        std::format_to(out, "BookDatabase (size = {}):\n", db.size());

        std::format_to(out, "Books:\n");
        for (const auto &book : db.GetBooks()) {
            std::format_to(out, "  - {}\n", book);
        }

        std::format_to(out, "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            std::format_to(out, "  - {}\n", author);
        }
        return out;
    }
};

}  // namespace std
