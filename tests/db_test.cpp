#include <gtest/gtest.h>

#include "book.hpp"
#include "book_database.hpp"
#include "comparators.hpp"
#include "statistics.hpp"

using namespace bookdb;
using namespace std::string_view_literals;

Book MakeTestBook(const std::string &title, const std::string &author, Genre genre, double rating, int year,
                  int read_count = 0) {
    return Book(title, author, year, genre, rating, read_count);
}

Book MakeTestBook(const std::string &title, const std::string &author, const std::string &genre, double rating,
                  int year, int read_count = 0) {
    return Book(title, author, year, genre, rating, read_count);
}

TEST(BookDatabaseTest, Clear) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Animal Farm", "Orwell", Genre::Fiction, 4.5, 1945));

    EXPECT_EQ(db.size(), 2);
    db.Clear();
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_TRUE(db.GetAuthors().empty());
}

TEST(BookDatabaseTest, PushBackLValue) {
    BookDatabase db;
    Book book = MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949);

    db.PushBack(book);

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].title, "1984");
    EXPECT_EQ(db.GetBooks()[0].author, "Orwell");
    EXPECT_EQ(db.GetBooks()[0].genre, Genre::SciFi);
    EXPECT_DOUBLE_EQ(db.GetBooks()[0].rating, 4.8);
    EXPECT_EQ(db.GetBooks()[0].year, 1949);
}

TEST(BookDatabaseTest, PushBackRValue) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].title, "1984");
}

TEST(BookDatabaseTest, PushBackMultipleBooks) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Brave New World", "Huxley", Genre::SciFi, 4.5, 1932));
    db.PushBack(MakeTestBook("Pride and Prejudice", "Austen", Genre::Fiction, 4.7, 1813));

    EXPECT_EQ(db.size(), 3);
    EXPECT_EQ(db.GetBooks()[0].title, "1984");
    EXPECT_EQ(db.GetBooks()[1].title, "Brave New World");
    EXPECT_EQ(db.GetBooks()[2].title, "Pride and Prejudice");
}

TEST(BookDatabaseTest, PushBackWithStringGenre) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", "SciFi", 4.8, 1949));

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].genre, Genre::SciFi);
}

TEST(BookDatabaseTest, EmplaceBack) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 0);

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].title, "1984");
    EXPECT_EQ(db.GetBooks()[0].author, "Orwell");
    EXPECT_EQ(db.GetBooks()[0].genre, Genre::SciFi);
}

TEST(BookDatabaseTest, EmplaceBackWithStringGenre) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, "SciFi", 4.8, 0);

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].genre, Genre::SciFi);
}

TEST(BookDatabaseTest, EmplaceBackMultiple) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 0);
    db.EmplaceBack("Animal Farm", "Orwell", 1945, Genre::Fiction, 4.5, 0);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 0);

    EXPECT_EQ(db.size(), 3);
}

TEST(BookDatabaseTest, AuthorNormalization) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Animal Farm", "Orwell", Genre::Fiction, 4.5, 1945));

    const auto &authors = db.GetAuthors();
    EXPECT_EQ(authors.size(), 1);

    const auto &book1 = db.GetBooks()[0];
    const auto &book2 = db.GetBooks()[1];
    EXPECT_EQ(book1.author.data(), book2.author.data());
    EXPECT_EQ(book1.author, "Orwell");
    EXPECT_EQ(book2.author, "Orwell");
}

TEST(BookDatabaseTest, MultipleAuthors) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Pride and Prejudice", "Austen", Genre::Fiction, 4.7, 1813));
    db.PushBack(MakeTestBook("Emma", "Austen", Genre::Fiction, 4.3, 1815));

    const auto &authors = db.GetAuthors();
    EXPECT_EQ(authors.size(), 2);
    EXPECT_TRUE(authors.contains("Orwell"));
    EXPECT_TRUE(authors.contains("Austen"));
}

TEST(BookDatabaseTest, AuthorNormalizationWithManyBooks) {
    BookDatabase db;

    for (int i = 0; i < 10; ++i) {
        db.EmplaceBack("Book" + std::to_string(i), "Orwell", 1949 + i, Genre::SciFi, 4.0 + i * 0.1, 0);
    }

    const auto &authors = db.GetAuthors();
    EXPECT_EQ(authors.size(), 1);
    EXPECT_TRUE(authors.contains("Orwell"));

    const auto &first_author = db.GetBooks()[0].author;
    for (const auto &book : db.GetBooks()) {
        EXPECT_EQ(book.author.data(), first_author.data());
    }
}

TEST(BookDatabaseTest, GetBooksNonConst) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949, 0));

    auto books = db.GetBooks();
    EXPECT_EQ(books.size(), 1);
}

TEST(BookDatabaseTest, GetAuthors) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Animal Farm", "Orwell", Genre::Fiction, 4.5, 1945));
    db.PushBack(MakeTestBook("Pride and Prejudice", "Austen", Genre::Fiction, 4.7, 1813));

    const auto &authors = db.GetAuthors();
    EXPECT_EQ(authors.size(), 2);
    EXPECT_TRUE(authors.find("Orwell") != authors.end());
    EXPECT_TRUE(authors.find("Austen") != authors.end());
}

TEST(BookDatabaseTest, GetAuthorsConst) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));

    const auto &const_db = db;
    const auto &authors = const_db.GetAuthors();
    EXPECT_EQ(authors.size(), 1);
    EXPECT_TRUE(authors.contains("Orwell"));
}

TEST(BookDatabaseTest, IteratorBeginEnd) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Animal Farm", "Orwell", Genre::Fiction, 4.5, 1945));

    auto it = db.begin();
    EXPECT_EQ(it->title, "1984");
    ++it;
    EXPECT_EQ(it->title, "Animal Farm");
    ++it;
    EXPECT_EQ(it, db.end());
}

TEST(BookDatabaseTest, ReverseIterator) {
    BookDatabase db;
    db.PushBack(MakeTestBook("Book1", "Author1", Genre::Fiction, 4.0, 2000));
    db.PushBack(MakeTestBook("Book2", "Author2", Genre::Fiction, 4.5, 2001));
    db.PushBack(MakeTestBook("Book3", "Author3", Genre::SciFi, 5.0, 2002));

    auto rit = db.rbegin();
    EXPECT_EQ(rit->title, "Book3");
    ++rit;
    EXPECT_EQ(rit->title, "Book2");
    ++rit;
    EXPECT_EQ(rit->title, "Book1");
    ++rit;
    EXPECT_EQ(rit, db.rend());
}

TEST(BookDatabaseTest, RangeBasedForLoop) {
    BookDatabase db;
    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    db.PushBack(MakeTestBook("Animal Farm", "Orwell", Genre::Fiction, 4.5, 1945));

    std::vector<std::string> titles;
    for (const auto &book : db) {
        titles.push_back(book.title);
    }

    EXPECT_EQ(titles.size(), 2);
    EXPECT_EQ(titles[0], "1984");
    EXPECT_EQ(titles[1], "Animal Farm");
}

TEST(BookDatabaseTest, EmptyAndSize) {
    BookDatabase db;
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);

    db.PushBack(MakeTestBook("1984", "Orwell", Genre::SciFi, 4.8, 1949));
    EXPECT_FALSE(db.empty());
    EXPECT_EQ(db.size(), 1);

    db.PushBack(MakeTestBook("Animal Farm", "Orwell", Genre::Fiction, 4.5, 1945));
    EXPECT_EQ(db.size(), 2);

    db.Clear();
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
}

TEST(GenreFromStringTest, ValidGenres) {
    EXPECT_EQ(GenreFromString("Fiction"), Genre::Fiction);
    EXPECT_EQ(GenreFromString("NonFiction"), Genre::NonFiction);
    EXPECT_EQ(GenreFromString("SciFi"), Genre::SciFi);
    EXPECT_EQ(GenreFromString("Biography"), Genre::Biography);
    EXPECT_EQ(GenreFromString("Mystery"), Genre::Mystery);
}

TEST(GenreFromStringTest, InvalidGenre) {
    EXPECT_EQ(GenreFromString("Unknown"), Genre::Unknown);
    EXPECT_EQ(GenreFromString("Fantasy"), Genre::Unknown);
    EXPECT_EQ(GenreFromString(""), Genre::Unknown);
}

TEST(GetTopNByTest, TopByAuthorWithComparator) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Animal Farm", "Orwell", 1945, Genre::Fiction, 4.5, 200);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Emma", "Austen", 1815, Genre::Fiction, 4.3, 120);
    db.EmplaceBack("Foundation", "Asimov", 1951, Genre::SciFi, 4.6, 250);

    auto top = getTopNBy(db, 3, comp::LessByAuthor{});

    EXPECT_EQ(top.size(), 3);
    EXPECT_EQ(top[0].get().author, "Orwell");
    EXPECT_EQ(top[1].get().author, "Orwell");
    EXPECT_EQ(top[2].get().author, "Huxley");
}

TEST(GetTopNByTest, TopByTitleWithComparator) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Animal Farm", "Orwell", 1945, Genre::Fiction, 4.5, 200);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Emma", "Austen", 1815, Genre::Fiction, 4.3, 120);

    auto top = getTopNBy(db, 3, comp::LessByTitle{});

    EXPECT_EQ(top.size(), 3);
    EXPECT_EQ(top[0].get().title, "Pride and Prejudice");
    EXPECT_EQ(top[1].get().title, "Emma");
    EXPECT_EQ(top[2].get().title, "Brave New World");
}

TEST(GetTopNByTest, TopByYearWithComparator) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Foundation", "Asimov", 1951, Genre::SciFi, 4.6, 250);

    auto top = getTopNBy(db, 2, comp::LessByYear{});

    EXPECT_EQ(top.size(), 2);
    EXPECT_EQ(top[0].get().year, 1951);
    EXPECT_EQ(top[1].get().year, 1949);
}

TEST(GetTopNByTest, TopByRatingWithComparator) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Emma", "Austen", 1815, Genre::Fiction, 4.3, 120);
    db.EmplaceBack("Foundation", "Asimov", 1951, Genre::SciFi, 4.6, 250);

    auto top = getTopNBy(db, 3, comp::LessByRating{});

    EXPECT_EQ(top.size(), 3);
    EXPECT_NEAR(top[0].get().rating, 4.8, 0.001);
    EXPECT_NEAR(top[1].get().rating, 4.7, 0.001);
    EXPECT_NEAR(top[2].get().rating, 4.6, 0.001);
}

TEST(GetTopNByTest, TopByPopularityWithComparator) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Emma", "Austen", 1815, Genre::Fiction, 4.3, 120);
    db.EmplaceBack("Foundation", "Asimov", 1951, Genre::SciFi, 4.6, 250);

    auto top = getTopNBy(db, 3, comp::LessByPopularity{});

    EXPECT_EQ(top.size(), 3);
    EXPECT_EQ(top[0].get().read_count, 300);
    EXPECT_EQ(top[1].get().read_count, 250);
    EXPECT_EQ(top[2].get().read_count, 180);
}

TEST(ComparatorTest, SortBooksByAuthor) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Emma", "Austen", 1815, Genre::Fiction, 4.3, 120);

    std::vector<Book> books(db.GetBooks().begin(), db.GetBooks().end());
    std::sort(books.begin(), books.end(), comp::LessByAuthor{});

    EXPECT_EQ(books[0].author, "Austen");
    EXPECT_EQ(books[1].author, "Austen");
    EXPECT_EQ(books[2].author, "Huxley");
    EXPECT_EQ(books[3].author, "Orwell");
}

TEST(ComparatorTest, SortBooksByYear) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);

    std::vector<Book> books(db.GetBooks().begin(), db.GetBooks().end());
    std::sort(books.begin(), books.end(), comp::LessByYear{});

    EXPECT_EQ(books[0].year, 1813);
    EXPECT_EQ(books[1].year, 1932);
    EXPECT_EQ(books[2].year, 1949);
}

TEST(ComparatorTest, SortBooksByTitle) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Animal Farm", "Orwell", 1945, Genre::Fiction, 4.5, 200);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);

    std::vector<Book> books(db.GetBooks().begin(), db.GetBooks().end());
    std::sort(books.begin(), books.end(), comp::LessByTitle{});

    EXPECT_EQ(books[0].title, "1984");
    EXPECT_EQ(books[1].title, "Animal Farm");
    EXPECT_EQ(books[2].title, "Brave New World");
}

TEST(ComparatorTest, SortBooksByGenre) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);
    db.EmplaceBack("Becoming", "Obama", 2018, Genre::Biography, 4.6, 250);

    std::vector<Book> books(db.GetBooks().begin(), db.GetBooks().end());
    std::sort(books.begin(), books.end(), comp::LessByGenre{});

    EXPECT_EQ(books[0].genre, Genre::Fiction);
    EXPECT_EQ(books[1].genre, Genre::SciFi);
    EXPECT_EQ(books[2].genre, Genre::Biography);
}

TEST(ComparatorTest, SortBooksByRating) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);

    std::vector<Book> books(db.GetBooks().begin(), db.GetBooks().end());
    std::sort(books.begin(), books.end(), comp::LessByRating{});

    EXPECT_NEAR(books[0].rating, 4.5, 0.001);
    EXPECT_NEAR(books[1].rating, 4.7, 0.001);
    EXPECT_NEAR(books[2].rating, 4.8, 0.001);
}

TEST(ComparatorTest, SortBooksByPopularity) {
    BookDatabase db;
    db.EmplaceBack("1984", "Orwell", 1949, Genre::SciFi, 4.8, 150);
    db.EmplaceBack("Brave New World", "Huxley", 1932, Genre::SciFi, 4.5, 180);
    db.EmplaceBack("Pride and Prejudice", "Austen", 1813, Genre::Fiction, 4.7, 300);

    std::vector<Book> books(db.GetBooks().begin(), db.GetBooks().end());
    std::sort(books.begin(), books.end(), comp::LessByPopularity{});

    EXPECT_EQ(books[0].read_count, 150);
    EXPECT_EQ(books[1].read_count, 180);
    EXPECT_EQ(books[2].read_count, 300);
}
