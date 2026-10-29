// ============================================================
// 第 7.1 课 · 章末练习（R 的答案版，仅供备课验证）
// ============================================================
#include "Book2.h"
#include <iostream>

Book2::Book2(string t, string i, double p, int pg)
{
    title = t;
    isbn = i;
    price = p;
    pages = pg;
}

bool Book2::checkISBN(string s)
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

void Book2::setPages(int p)
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

string Book2::getIsbn() { return isbn; }

void Book2::showInfo()
{
    cout << "《" << title << "》 ISBN：" << isbn
         << "  " << price << " 元, " << pages << " 页" << endl;
}
