// ============================================================
// 第 9.1 课 · 章末练习（R 的答案版，仅供备课验证）
// ============================================================
#include "Library2.h"
#include <iostream>

Library2::Library2(string n)
{
    name = n;
}

void Library2::addBook(Book b)
{
    books.push_back(b);
    cout << "入库：《" << b.getTitle() << "》" << endl;
}

int Library2::count()
{
    return (int)books.size();
}

void Library2::showAll()
{
    cout << "== " << name << " 共 " << count() << " 本 ==" << endl;
    for (int i = 0; i < count(); i++)
    {
        books[i].showInfo();
    }
}

void Library2::findBook(string isbn)
{
    for (int i = 0; i < count(); i++)
    {
        if (books[i].getIsbn() == isbn)
        {
            cout << "找到：《" << books[i].getTitle() << "》" << endl;
            return;
        }
    }
    cout << "没找到 ISBN 为 " << isbn << " 的书" << endl;
}
