// ============================================================
// Book.h —— 类的"声明"：只说不做
// ------------------------------------------------------------
// 第 10 课新增：在馆状态（borrowed）+ 借出/归还/可借判断
// ============================================================
#pragma once

#include <string>
using namespace std;

class Book {
private:
    string title;
    string isbn;
    double price;
    int pages;
    bool borrowed;                                 // 第 10 课：在馆状态（false = 在馆，true = 已借出）

public:
    Book();                                        // 默认构造函数
    Book(string t, string i, double p, int pg);    // 带参构造函数

    bool checkISBN(string s);
    void setPages(int p);
    string getTitle();
    string getIsbn();
    double getPrice();                             // 第 15 课：存文件要用
    int getPages();                                // 第 15 课：存文件要用

    bool isAvailable();                            // 第 10 课：还能借吗
    void borrowOne();                              // 借出
    void returnOne();                              // 归还

    void showInfo();
};
