// ============================================================
// Book2.h —— 第 7.1 课练习用（声明部分，完整，不要动）
// ============================================================
#pragma once

#include <string>
using namespace std;

class Book2 {
private:
    string title;
    string isbn;
    double price;
    int pages;

public:
    Book2(string t, string i, double p, int pg);
    bool checkISBN(string s);
    void setPages(int p);
    string getIsbn();
    void showInfo();
};
