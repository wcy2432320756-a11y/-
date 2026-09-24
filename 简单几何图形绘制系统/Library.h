#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <vector>

#include "Book.h"
#include "Reader.h"

class Library {
private:
    std::vector<Book> books;
    std::vector<Reader> readers;
    const std::string bookFile = "books.txt";
    const std::string readerFile = "readers.txt";

    int findBookIndex(const std::string& isbn) const;
    int findReaderIndex(const std::string& id) const;
    void load();
    void save() const;

public:
    Library();
    ~Library();

    void addBook();
    void showBooks() const;
    void addReader();
    void showReaders() const;
    void borrowBook();
    void returnBook();
    void searchBook() const;
    void menu();
};

#endif
