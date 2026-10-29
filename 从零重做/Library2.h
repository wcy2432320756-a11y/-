// ============================================================
// Library2.h —— 第 9.1 课练习用（声明部分，完整，不要动）
// ============================================================
#pragma once

#include <string>
#include <vector>
#include "Book.h"
using namespace std;

class Library2 {
private:
    string name;
    vector<Book> books;

public:
    Library2(string n);
    void addBook(Book b);
    int count();
    void showAll();
    void findBook(string isbn);
};
