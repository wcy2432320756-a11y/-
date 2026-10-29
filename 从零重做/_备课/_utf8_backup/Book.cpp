// ============================================================
// Book.cpp —— 类的"定义"：真正干活的代码
// ------------------------------------------------------------
// 每个函数前面加 Book::，意思是"这是 Book 的成员函数"。
// ============================================================
#include "Book.h"
#include <iostream>

Book::Book()                       // 默认构造函数
{
    title = "未命名";
    isbn = "";
    price = 0.0;
    pages = 0;
    borrowed = false;
}

Book::Book(string t, string i, double p, int pg)
{
    title = t;
    isbn = i;
    price = p;
    pages = pg;
    borrowed = false;
}

bool Book::checkISBN(string s)
{
    int n = (int)s.size();
    if (n != 13) return false;

    int sum = 0;
    for (int i = 0; i < 13; i++) {
        int d = s[i] - '0';
        if (d < 0 || d > 9) return false;
        if (i % 2 == 0) sum = sum + d * 1;
        else            sum = sum + d * 3;
    }
    return sum % 10 == 0;
}

void Book::setPages(int p)
{
    if (p > 0)
    {
        pages = p;
    }
    else
    {
        cout << "【拒收】页数必须大于 0：" << p << endl;
    }
}

string Book::getTitle() { return title; }

string Book::getIsbn() { return isbn; }

bool Book::isAvailable() { return !borrowed; }   // 没借出 = 可借

void Book::borrowOne() { borrowed = true; }

void Book::returnOne() { borrowed = false; }

void Book::showInfo()
{
    cout << "《" << title << "》 ISBN：" << isbn
         << "  " << price << " 元, " << pages << " 页，";
    if (borrowed) cout << "已借出";
    else          cout << "在馆";
    cout << endl;
}
