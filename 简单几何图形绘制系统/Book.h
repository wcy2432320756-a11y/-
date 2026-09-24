#ifndef BOOK_H
#define BOOK_H
#include <string>

class Book {
public:
    std::string isbn, title, author;
    int total, available;

    Book();
    Book(std::string i, std::string t, std::string a, int tot, int av);
};

#endif
